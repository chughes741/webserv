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
            // Logger::instance().log("Creating socket for server: " + it->listen.first + ":" +
                                //    std::to_string(it->listen.second));

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

    HttpRequest request(buffer_pair.first, sessions_[session_id]);

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

bool HttpServer::deleteMethod(HttpRequest &request, HttpResponse &response,
                           ServerConfig &server, LocationConfig *location) {
    (void) request;
    (void) response;
    (void) server;
    (void) location;
    Logger::instance().log("Delete method activated");
    response.status_ = OK;
    return true;
}

bool HttpServer::postMethod(HttpRequest &request, HttpResponse &response, ServerConfig &server,
                            LocationConfig *location) {
    (void)server;
    (void)location;

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

    } else if (request.headers_["Content-Type"] == "application/x-www-form-urlencoded") {
        Logger::instance().log("POST: Returning response from form-data");
        response.headers_["Content-Type"] = "text/html; charset=utf-8";

        if (true) {
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

// Find the appropriate location and fill the response body
bool HttpServer::buildResponse(HttpRequest &request, HttpResponse &response,
                           ServerConfig &server) {
    LocationConfig *location = NULL;
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
    if (checkIfDirectoryRequest(request, location, server)) {
        if (checkForIndexFile(request, location, server)) {
            handleIndexFile(request, response, location, server);
        }
        else if (location->autoindex) {
            generateDirectoryListing(request, response, location, server);
        }
        else { //if autoindex is disabled and the request is for a directory by default server will return an error 403
            handleForbidden(response, location, server);
        }
        return true;
    }
    else if (location->cgi_enabled && checkUriForExtension(request.uri_, location)) { //cgi handling before. Unsure if it should stay here or be handle within getMethod or postMethod
        Logger::instance().log("Enter cgi");
        Logger::instance().log(request.printRequest());
        Cgi newCgi(request, *location, server, response);
        return newCgi.exec();
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
    // Logger::instance().log(request.printRequest());

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

void HttpServer::handleForbidden(HttpResponse &response, LocationConfig *location, ServerConfig &server) { //in case requested directory does not have autoindex enabled
    std::string errorPage;
    std::string path;
    std::map<int, std::string>::iterator it;

    if ((it = location->error_page.find(FORBIDDEN)) != location->error_page.end()) { //checks if an error page corresponding to a 403 status code exists at the location level
        path = it->second;
        std::string root;
	    if (location->root.size() != 0) {
		    root = location->root;
		    root.append("/");
	    }
	    else {
		    root = config_.root;
		    root.append("/");
	    }
	    root.append(path);
	    std::ifstream in(root);
        std::stringstream buffer;
        buffer << in.rdbuf();
        errorPage = buffer.str();
        in.close();
    }
    else if ((it = server.error_page.find(FORBIDDEN)) != server.error_page.end()) { //checks if an error page corresponding to a 403 status code exists at the server level
        path = it->second;
        std::string root;
	    if (location->root.size() != 0) {
		    root = location->root;
		    root.append("/");
	    }
	    else {
		    root = config_.root;
		    root.append("/");
	    }
	    root.append(path);
	    std::ifstream in(root);
        std::stringstream buffer;
        buffer << in.rdbuf();
        errorPage = buffer.str();
        in.close();
    }
    else { //default 403 error page
        errorPage = "<html><head><style>body{display:flex;justify-content:center;align-items:center;height:100vh;margin:0;}.error-message{text-align:center;}</style></head><body><div class=\"error-message\"><h1>Homemade Webserv</h1><h1>403 Forbidden</h1></div></body></html>";
    }
    response.body_ = errorPage;
    response.status_ = FORBIDDEN;
    response.headers_["content-type"] = "text/html";
}



void HttpServer::handleIndexFile(HttpRequest &request, HttpResponse &response, LocationConfig *location, ServerConfig &server) {
    std::string tempUri;
    if (location) {
        if (location->root.size()) { //check if root is set at the location level
            tempUri.append(location->root);
        }
        else { //fallback to server root directive
            tempUri.append(server.root);
        }
        tempUri.append(request.uri_);
        tempUri.append("index.html");
        if (readFileToBody(response, tempUri) == true) {
            std::cout << "read file worked" << std::endl;
            response.headers_["content-type"] = "text/html";
            response.status_ = OK;
        }
        else { //something went wrong with reading index.html file
            return ;
        }
    }
}

bool HttpServer::checkIfDirectoryRequest(HttpRequest &request, LocationConfig *location, ServerConfig &server) { //used to check if request is simply for a directory
    std::string tempUri;
    if (location->root.size()) { //check if root is set at the location level
        tempUri.append(location->root);
    }
    else { //fallback to server root directive
        tempUri.append(server.root);
    }
    tempUri.append(request.uri_);

    DIR *currentDirectory = opendir(tempUri.c_str()); //attempt to open the directory specified. If successful then it means the request was indeed for a directory.
    if (!currentDirectory) {
        return (false);
    }
    if (closedir(currentDirectory)) { //to prevent leaks. Cause leaks suck
        std::string error("Call to closedir failed: ");
        error.append(strerror(errno));
        Logger::instance().log(error);
    }
    return true;
}

bool HttpServer::checkForIndexFile(HttpRequest &request, LocationConfig *location, ServerConfig &server) {
    std::vector<std::pair<unsigned char, std::string> > files = returnFiles(request, location, server);
    for (std::size_t i = 0; i < files.size(); ++i) {
        if (files[i].second == "index.html") { //checks the name of the file/folder
            if (files[i].first == DT_REG) { //checks if it is actually a file
                return true; //yay a file called index.html exists!
            }
        }
    }
    return false; //got to generate an html document with the files contained within the requested directory
}

void HttpServer::generateDirectoryListing(HttpRequest &request, HttpResponse &response, LocationConfig *location, ServerConfig &server) {
    std::string responseBody;
    std::vector<std::pair<unsigned char, std::string> > currentDir = returnFiles(request, location, server);
    std::vector<std::string> directories;
    std::vector<std::string> files;

    for (std::size_t i = 0; i < currentDir.size(); ++i) {
        if (currentDir[i].first == DT_DIR) {
            if (currentDir[i].second != ".")
                directories.push_back(currentDir[i].second);
        }
        if (currentDir[i].first == DT_REG) {
            files.push_back(currentDir[i].second);
        }
    }

    if (!hasTrailingSlash(request)) {
        std::string newLocation("http://");
        std::string host;
        std::map<std::string, std::string>::iterator it = request.headers_.find("Host");
        if (it != request.headers_.end()) {
            newLocation.append(it->second);
        }
        newLocation.append(request.uri_);
        newLocation.append("/");
        response.status_ = MOVED_PERMANENTLY;
        response.headers_["Location"] = newLocation;
    }
    else {
        response.status_ = OK;
    }
    response.headers_["content-type"] = "text/html";

    responseBody.append("<!doctype html><html><head><title>Index of ");
    responseBody.append(request.uri_);
    responseBody.append("</title></head><body><h1>Index of ");
    responseBody.append(request.uri_);
    responseBody.append("</h1><hr><pre>");
    for (std::size_t i = 0; i < directories.size(); ++i) {
        responseBody.append("<a href=\"");
        responseBody.append(directories[i]);
        responseBody.append("/\">");
        responseBody.append(directories[i]);
        responseBody.append("/</a>\n");
    }
    for (std::size_t i = 0; i < files.size(); ++i) {
        responseBody.append("<a href=\"");
        responseBody.append(files[i]);
        responseBody.append("\">");
        responseBody.append(files[i]);
        responseBody.append("</a>\n");
    }
    responseBody.append("</pre><hr></body></html>");
    response.body_ = responseBody;
}

std::vector<std::pair<unsigned char, std::string> > HttpServer::returnFiles(HttpRequest &request, LocationConfig *location, ServerConfig &server) {
    std::string tempUri;
    std::vector<std::pair<unsigned char, std::string> > files;
    struct dirent *file;
    if (location->root.size()) { //check if root is set at the location level
        tempUri.append(location->root);
    }
    else { //fallback to server root directive
        tempUri.append(server.root);
    }
    tempUri.append(request.uri_);

    DIR *currentDirectory = opendir(tempUri.c_str());
    do {
        file = readdir(currentDirectory);
        if (file != nullptr) {
            files.push_back(std::make_pair(file->d_type, file->d_name));
        }
    }
    while (file != nullptr);
    if (closedir(currentDirectory)) {
        std::string error("Something went wrong with closedir: ");
        error.append(strerror(errno));
        Logger::instance().log(error);
    }
    return files;
}

bool HttpServer::hasTrailingSlash(HttpRequest &request) const {
    if (request.uri_[request.uri_.size() - 1] == '/') {
        return true;
    }
    return false;
}

void HttpServer::addTrailingSlash(HttpRequest &request, HttpResponse &response) {
    std::string newUri = request.uri_;
    newUri.append("/");
    response.headers_["Location"] = newUri;
    response.status_ = MOVED_PERMANENTLY;
}