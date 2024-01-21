#include "server.hpp"
#include "cgi.hpp"

extern HttpConfig httpConfig;

HttpServer::HttpServer(HttpConfig httpConfig, SocketGenerator socket_generator)
    : socket_generator_(socket_generator), config_(httpConfig) {}

HttpServer::~HttpServer() {}

void HttpServer::start(bool run_server) {
    Logger::instance().log("Starting server");

    // Set up signal handlers
    listener_.registerEvent(SIGINT, SIGNAL_EVENT);
    listener_.registerEvent(SIGTERM, SIGNAL_EVENT);

    // Create a socket for each server in the config
    Socket *new_socket;
    for (std::vector<ServerConfig>::iterator it = config_.servers.begin();
         it != config_.servers.end(); ++it) {
        try {
            Logger::instance().log("Creating socket for server: http://" + it->listen.first + ":" +
                                   std::to_string(it->listen.second));

            // Create a new socket
            new_socket = socket_generator_();

            // Bind the socket to the address/port
            int server_id = new_socket->bind(it->listen.first, it->listen.second);

            // Listen for connections
            new_socket->listen();

            // Add the socket to the map
            server_sockets_[server_id] = new_socket;

            // Add the socket to the listener
            listener_.registerEvent(server_id, READABLE);

        } catch (std::bad_alloc &e) {
            Logger::instance().log(e.what());
        } catch (std::exception &e) {
            Logger::instance().log(e.what());
            delete new_socket;
        }
    }

    // Run the server
    if (run_server == true) run();
}

void HttpServer::stop() {
    Logger::instance().log("Stopping server");

    // Close all sockets and delete them
    for (std::map<int, Socket *>::iterator it = server_sockets_.begin();
         it != server_sockets_.end(); ++it) {
        try {
            it->second->close();
        } catch (std::runtime_error &e) {
            Logger::instance().log(e.what());
        }
        delete it->second;
    }

    // Clear the map of sockets
    server_sockets_.clear();
}

void HttpServer::run() {
    Logger::instance().log("Running server");

    // Loop forever
    while (true) {
        // Wait for an event
        std::pair<int, InternalEvent> event = listener_.listen();

        // Handle event
        if (server_sockets_.find(event.first) != server_sockets_.end()) {
            connectHandler(event.first);

        } else {
            switch (event.second) {
                case NONE:
                    break;
                case READABLE:
                    readableHandler(event.first);
                    break;
                case WRITABLE:
                    writableHandler(event.first);
                    break;
                case ERROR_EVENT:
                    errorHandler(event.first);
                    break;
                case SIGNAL_EVENT:
                    signalHandler(event.first);
                    break;
            }
        }
    }
}

void HttpServer::readableHandler(int session_id) {
    // Logger::instance().log("Received request on fd: " + std::to_string(session_id));

    // Receive the request
    std::pair<HttpRequest, ssize_t> request = receiveRequest(session_id);

    // Logger::instance().log(request.first.printRequest());

    if (request.second == 0) {
        disconnectHandler(session_id);
        return;
    }

    // Handle the request
    HttpResponse response = handleRequest(request.first);

    // Add the response to the clients send queue
    sessions_[session_id]->addSendQueue(response.getMessage());

    // Add the session to the listener
    listener_.registerEvent(session_id, WRITABLE);
}

void HttpServer::writableHandler(int session_id) {
    // Logger::instance().log("Sending response on fd: " + std::to_string(session_id));

    if (sessions_[session_id]->send()) {
        listener_.unregisterEvent(session_id, WRITABLE);
    }
}

void HttpServer::errorHandler(int session_id) {
    Logger::instance().log("Error on fd: " + std::to_string(session_id));
    return;
}

void HttpServer::signalHandler(int signal) {
    // Logger::instance().log("Received signal: " + std::to_string(signal));

    if (signal == SIGINT || signal == SIGTERM) {
        stop();
    }
}

void HttpServer::connectHandler(int socket_id) {
    // Logger::instance().log("Received connection on fd: " + std::to_string(socket_id));

    // Accept the connection
    Session *session = server_sockets_[socket_id]->accept();

    // Create a new session
    sessions_[session->getSockFd()] = session;

    // Add the session to the listener
    listener_.registerEvent(session->getSockFd(), READABLE); /** @todo event flags */
}

void HttpServer::disconnectHandler(int session_id) {
    // Logger::instance().log("Disconnecting fd: " + std::to_string(session_id));

    // Remove the session from the listener
    listener_.unregisterEvent(session_id, READABLE | WRITABLE);

    // Delete the session
    delete sessions_[session_id];

    // Remove the session from the map
    sessions_.erase(session_id);

    // Close the socket
    close(session_id);
}

std::pair<HttpRequest, ssize_t> HttpServer::receiveRequest(int session_id) {
    std::pair<std::string, ssize_t> buffer_pair = sessions_[session_id]->recv(session_id);

    HttpRequest request(buffer_pair.first);

    return std::make_pair(request, buffer_pair.second);
}

bool isResourceRequest(HttpResponse &response, const std::string &uri) {
    if (uri.size() >= 4 && uri.substr(uri.size() - 4) == ".css") {
        response.headers_["Content-Type"] = "text/css";
        return true;
    }
    if (uri.size() >= 3 && uri.substr(uri.size() - 3) == ".js") {
        response.headers_["Content-Type"] = "text/javascript";
        return true;
    }
    return false;
}

// Build the error page : ToDo -> Make it take the error code
bool HttpServer::buildNotFound(HttpRequest &request, HttpResponse &response, ServerConfig &server,
                               LocationConfig *location) {
    response.status_ = NOT_FOUND;
    std::string root = location && location->root.size() > 0 ? location->root : server.root;
    if (isResourceRequest(response, request.uri_)) {
        std::string resource = root + request.uri_;
        response.status_ = OK;
        return readFileToBody(response, resource);
    }
    std::string *errorPath = NULL;
    if (location) {
        std::map<int, std::string>::iterator it = location->error_page.find(404);
        if (it != location->error_page.end())
            errorPath = &it->second;
    }
    if (!errorPath) {
        std::map<int, std::string>::iterator it = server.error_page.find(404);
        if (it != server.error_page.end())
            errorPath = &it->second;
    }
    if (!errorPath)
        return false;
    std::string filepath = root + '/' + *errorPath;
    return readFileToBody(response, filepath);
}

// Read a file into the response body
bool HttpServer::readFileToBody(HttpResponse &response, std::string &filepath) {
    std::ifstream in(filepath);
    if (!in)
        return false;
    std::stringstream buffer;
    buffer << in.rdbuf();
    response.body_ = buffer.str();
    in.close();
    return true;
}

std::string HttpServer::trimHost(const std::string &uri, ServerConfig &server) {
    size_t startPos = uri.find(std::to_string(server.listen.second));
    if (startPos != std::string::npos) {
        startPos += std::to_string(server.listen.second).size();
        return uri.substr(startPos);
    }
    return uri;
}

bool deleteFile(const std::string &filename) {
    std::string filePath = "uploads/" + filename;
    return std::remove(filePath.c_str()) == 0;
}

void uploadsFileList(std::stringstream &fileList) {
    fileList << "<script>"
         << "function handleDeleteButtonClick(filename) {"
         << "    var currentUrl = window.location.href + 'delete?filename=' + encodeURIComponent(filename);"
         << "    fetch(currentUrl, {"
         << "        method: 'DELETE',"
         << "        headers: { 'Content-Type': 'application/json' },"
         << "    })"
         << "    .then(response => {"
         << "        if (response.ok) {"
         << "            console.log('DELETE request for ' + filename + ' successful');"
         << "        } else {"
         << "            console.error('DELETE request for ' + filename + ' failed');"
         << "        }"
         << "    })"
         << "    .catch(error => {"
         << "        console.error('Error during DELETE request for ' + filename + ':', error);"
         << "    });"
         << "}"
         << "</script>";
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("uploads")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                std::string filename = std::string(ent->d_name);
                fileList << "<li style=\"clear: both;\">"
                         << "<a href=\"/display?filename=" << filename << "\">" << filename << "</a>"
                         << "<form id=\"deleteForm" << filename << "\" style=\"float: right;\">"
                         << "<input type=\"hidden\" name=\"filename\" value=\"" << filename << "\">"
                         << "<input type=\"button\" value=\"Delete\" onclick=\"handleDeleteButtonClick('" << filename << "');\">"
                         << "</form>"
                         << "</li>";
            }
        }
        closedir(dir);
    }
}

std::string urlDecode(const std::string& str) {
    std::stringstream decoded;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            char decodedChar = static_cast<char>(std::strtoul(str.substr(i + 1, 2).c_str(), nullptr, 16));
            decoded << decodedChar;
            i += 2;
        } else if (str[i] == '+') {
            decoded << ' ';
        } else {
            decoded << str[i];
        }
    }
    return decoded.str();
}

bool HttpServer::deleteMethod(HttpRequest &request, HttpResponse &response,
                           ServerConfig &server, LocationConfig *location) {
    (void) server;
    (void) location;
    
    size_t position = request.uri_.find("=");
    std::string encodedFilename = request.uri_.substr(position + 1);
    std::string filename = urlDecode(encodedFilename);
    deleteFile(filename);
    std::stringstream fileList;
    uploadsFileList(fileList);
    response.body_ = "<html><body><h2>Uploads:</h2><ul>" + fileList.str() + "</ul>" + "<a href='/'>Return Home</a></body></html>";
    Logger::instance().log("Delete method activated");
    response.status_ = OK;
    return true;
}

std::string extractValue(const std::string& data, const std::string& start, const std::string& end) {
    size_t startPos = data.find(start) + start.length();
    size_t endPos = data.find(end, startPos);
    if (end.empty()) {
        endPos = data.length();
    }
    return data.substr(startPos, endPos - startPos);
}

bool fileExists(const std::string &filePath) {
    std::ifstream file(filePath.c_str());
    return file.good();
}

std::string generateUniqueFileName(std::string &originalFileName) {
    std::string newFileName = "uploads/" + originalFileName;
    std::string nameWithoutExtension;
    size_t dot_position;
    bool moreThanOneChange = false;
    int counter = 1;
    while(fileExists(newFileName)) {
        ++counter;
        if (counter > 2) {
            moreThanOneChange = true;
        }
        dot_position = newFileName.find('.');
        nameWithoutExtension = newFileName.substr(0, dot_position);
        if (moreThanOneChange == true) {
            dot_position = originalFileName.find('.'); 
            newFileName = "uploads/" + originalFileName.substr(0, dot_position) + "_" + std::to_string(counter) + originalFileName.substr(dot_position);
        }
        else
            newFileName = nameWithoutExtension + "_" + std::to_string(counter) + newFileName.substr(dot_position);
    }
    return (newFileName);
}

std::string readFileContent(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (file) {
        std::stringstream content;
        content << file.rdbuf();
        file.close();
        return content.str();
    }
    else {
        return "Error reading file: " + filename;
    }
}

bool displayFile(HttpRequest& request, HttpResponse& response) {
    std::string filename;
    size_t pos = request.uri_.find('?');
    if (pos != std::string::npos) {
        filename = request.uri_.substr(pos + 1);
    }

    std::string filePath = "uploads/" + filename;

    std::ifstream file(filePath.c_str());
    if (!file.is_open()) {
        response.status_ = NOT_FOUND;
        response.body_ = "File not found";
        return false;
    }
    file.close();

    std::string fileContent = readFileContent(filePath);

    response.headers_["Content-Type"] = "application/octet-stream";
    response.headers_["Content-Disposition"] = "attachment; filename=\"" + filename + "\"";

    response.body_ = fileContent;

    response.status_ = OK;
    response.headers_["Content-Length"] = std::to_string(response.body_.size());

    return true;
}

bool HttpServer::postMethod(HttpRequest &request, HttpResponse &response, ServerConfig &server,
                            LocationConfig *location) {
    (void)server;
    (void)location;

    if (request.body_.empty()) {
            response.status_ = BAD_REQUEST;
    } 

    else {
        std::string userInput;
        size_t pos = request.body_.find("text_input=");
        if (pos != std::string::npos) {
            userInput = request.body_.substr(pos + 11);
            if (userInput.empty()) {
                response.body_ = "<html><body>This field cannot be empty<br><br><a href='/'>Return Home</a></body></html>";
                return true;
            }
            else {
                response.body_ = "<html><body>You've entered: " + userInput + "<br><br><a href='/'>Return Home</a></body></html>";
                return true;
            }
        }
        response.status_ = OK;
        response.headers_["Content-Type"] = "text/html";
        response.headers_["Content-Length"] = std::to_string(response.body_.size());
    }

    if (request.headers_["Content-Type"] == "text/plain") {
        Logger::instance().log("POST: Creating file with text data");
        response.headers_["Content-Type"] = "text/plain; charset=utf-8";

        // TODO: Create a file with the body data

        if (true) {  // TODO: Check the return code of the file creation
            response.status_ = CREATED;
            response.body_   = "File created successfully";

        } else {
            response.status_ = INTERNAL_SERVER_ERROR;
            response.body_   = "Error creating file";
        }

    } else if (request.headers_["Content-Type"].find("multipart/form-data") != std::string::npos) {

        std::string boundary = extractValue(request.headers_["Content-Type"], "boundary=", "");
        if (request.body_.empty()) {
            std::cerr << "Problem uploading the file" << std::endl;
            response.body_ = "<html><body>There was an error uploading the file<br><br><a href='/'>Return Home</a></body></html>";
            return true;
        }

        std::string delimiter = "--" + boundary;
        size_t pos = 0;
        pos = request.body_.find(delimiter);
        while (pos != std::string::npos) {
            size_t endPos = request.body_.find(delimiter, pos + delimiter.length());
            
            std::string part = request.body_.substr(pos, endPos - pos);

            size_t filenamePos = part.find("filename=\"");
            if (filenamePos != std::string::npos) {
                std::string filename = extractValue(part, "filename=\"", "\"");
                if (filename.empty()) {
                    break;
                }
                std::string realFileName = generateUniqueFileName(filename);
                std::cout << "FILENAME= " << realFileName << std::endl;
                size_t contentPos = part.find("\r\n\r\n") + 4;
                std::ofstream file(realFileName.c_str(), std::ios::binary | std::ios::app);
                std::string content = part.substr(contentPos, part.length() - contentPos - 2);
                file << content;
                file.close();
                std::cout << "File '" << realFileName << "' uploaded successfully to /uploads" << std::endl; 
                
            }
            pos = response.body_.find(delimiter, endPos);
        }
    }

    
    else if (request.headers_["Content-Type"] == "application/x-www-form-urlencoded") {
        Logger::instance().log("POST: Returning response from form-data");
        response.headers_["Content-Type"] = "text/html; charset=utf-8";

        // TODO: Call the CGI script



        if (true) {  // TODO: Check the return code of the CGI script
            response.status_ = OK;

        } else {
            response.status_ = INTERNAL_SERVER_ERROR;
        }

    } else {
        Logger::instance().log("POST: Content-Type not supported");
        response.status_                  = BAD_REQUEST;
        response.headers_["Content-Type"] = "text/plain; charset=utf-8";
        response.body_                    = "Content-Type not supported";
    }

    std::stringstream fileList;
    uploadsFileList(fileList);
    
    response.body_ = "<html><body><h2>Uploads:</h2><ul>" + fileList.str() + "</ul>" + "<a href='/'>Return Home</a></body></html>";
    response.status_ = OK;
    response.headers_["Content-Type"] = "text/html";
    response.headers_["Content-Length"] = std::to_string(response.body_.size());

    return true;
}

bool HttpServer::getMethod(HttpRequest &request, HttpResponse &response,
                           ServerConfig &server, LocationConfig *location) {
    response.headers_["Content-Type"] = "text/html; charset=utf-8";
    if (location) {     
        if (!isResourceRequest(response, request.uri_) && location->autoindex)
            request.uri_ = request.uri_ + location->index_file;
        std::string root = location->root.length() > 0 ? location->root : server.root;
        std::string filepath = isResourceRequest(response, request.uri_) ? request.uri_ : root + request.uri_;
        if (readFileToBody(response, filepath)) {
            response.status_ = OK;
            return true;
        }
    }
    return buildNotFound(request, response, server, location);
}

bool HttpServer::validateRequestBody(HttpRequest &request, ServerConfig &server, LocationConfig *location) {
    size_t max = location->client_max_body_size;
    if (location->max_body_size) {
        max = location->client_max_body_size;
    } else if (server.max_body_size) {
        max = server.client_max_body_size;
    } else if (this->config_.max_body_size) {
        max = this->config_.client_max_body_size;
    }
    return request.body_.size() <= max;
}

bool HttpServer::buildBadRequestBody(HttpResponse &response) {
    response.status_ = CONTENT_TOO_LARGE;
    response.headers_["Connection"] = "close";
    return true;
}

bool HttpServer::isRedirectServer(HttpRequest &request, HttpResponse &response, ServerConfig &server) {
    if (server.redirect.first == 0)
        return false;
    response.status_ = HttpStatus(server.redirect.first);
    size_t pos = server.redirect.second.find("$request_uri");
    if (pos != std::string::npos) {
        response.headers_["Location"] = server.redirect.second.substr(0, pos) + request.uri_;
    } else {
        response.headers_["Location"] = server.redirect.second;
    }
    response.headers_["Content-Length"] = std::to_string(response.body_.size());
    return true;
}

// Find the appropriate location and fill the response body
bool HttpServer::buildResponse(HttpRequest &request, HttpResponse &response,
                           ServerConfig &server) {
    LocationConfig *location = NULL;
    if (isRedirectServer(request, response, server)) {
        return true;
    }
    std::string uri = isResourceRequest(response, request.uri_) ? trimHost(request.headers_["Referer"], server) : request.uri_;
    // Logger::instance().log("uri: " + uri);
    for (std::map<std::string, LocationConfig>::iterator it = server.locations.begin();
     it != server.locations.end(); ++it) {    
        if (uri.substr(0, it->first.size()) == it->first) {
            location = &(it->second);
        }
    }
    // Logger::instance().log(request.printRequest());
    if (!location) {
        return buildNotFound(request, response, server, location);
    } else if (!(location->limit_except & request.method_)) {
        response.status_ = BAD_REQUEST;
        return true;
    } else if (!validateRequestBody(request, server, location)) {
        return buildBadRequestBody(response);
    }
    if (location->cgi_enabled && checkUriForExtension(request.uri_, location)) { //cgi handling before. Unsure if it should stay here or be handle within getMethod or postMethod
        Logger::instance().log("Enter cgi");
        Logger::instance().log(request.printRequest());
        Cgi newCgi(request, *location, server, response);
        bool result = newCgi.exec();
        return result;
    }
    else {
        switch (request.method_) {
        // @todo verify if method is allowed on location
        case 1: // Enums for comparisons is C++11...
            return getMethod(request, response, server, location);
        case 2: // Enums for comparisons is C++11...
            return postMethod(request, response, server, location);
        case 3: // Enums for comparisons is C++11...
            return deleteMethod(request, response, server, location);
        default:
            return false;
        }
    }
}

// Validate the host making the request is in the servers
bool HttpServer::validateHost(HttpRequest &request, HttpResponse &response) {
    std::string requestHost = request.headers_["Host"];  // Check if the host is valid

    for (std::vector<ServerConfig>::iterator it = config_.servers.begin();
         it != config_.servers.end(); ++it) {
        std::string serverHost = it->listen.first + ":" + std::to_string(it->listen.second);
        std::string localHost = "localhost:" + std::to_string(it->listen.second);
        if (requestHost == serverHost || requestHost == localHost) {
            return buildResponse(request, response, *it);
        }
    }
    return false;
}

HttpResponse HttpServer::handleRequest(HttpRequest request) {
    HttpResponse response;
    //Logger::instance().log(request.printRequest());

    response.version_ = HTTP_VERSION;
    response.server_  = "webserv/0.1";
    response.headers_["Connection"] = "Keep-Alive";

    if (request.version_ != "HTTP/1.1") {
        response.status_ = IM_A_TEAPOT; // If this happen we ignore the request and return an empty answer
    } 
    else if (request.method_ == 0){ // Enums for comparisons is C++11...
        response.status_                  = BAD_REQUEST;
    }
    else if (!validateHost(request, response)) {
        response.status_                  = NOT_FOUND;
        response.headers_["Content-Type"] = "text/html";
        response.body_ = "<html><head><style>body{display:flex;justify-content:center;align-items:center;height:100vh;margin:0;}.error-message{text-align:center;}</style></head><body><div class=\"error-message\"><h1>Homemade Webserv</h1><h1>404 Not Found</h1></div></body></html>";
    }
    if (response.body_.size() > 0) {
        response.headers_["content-length"] = std::to_string(response.body_.size());
    }
    return response;
}

bool HttpServer::checkUriForExtension(std::string& uri, LocationConfig *location) const {
    std::string ext;

	for (size_t i = 0; i < location->cgi_ext.size(); ++i) {
		if (uri.find(location->cgi_ext[i]) != std::string::npos) {
			ext = location->cgi_ext[i];
		}
	}
	if (!ext.size())
		return false;
    else
        return true;
}