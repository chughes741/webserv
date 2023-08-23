#include "server.hpp"

extern HttpConfig httpConfig;

HttpServer::HttpServer(HttpConfig httpConfig, EventListener *listener,
                       SocketGenerator socket_generator)
    : socket_generator_(socket_generator), listener_(listener), config_(httpConfig) {}

HttpServer::~HttpServer() {}

void HttpServer::start(bool run_server) {
    // Set up signal handlers
    listener_->registerEvent(SIGINT, SIGNAL_EVENT);
    listener_->registerEvent(SIGTERM, SIGNAL_EVENT);

    // Create a socket for each server in the config
    Socket *new_socket;
    for (std::vector<ServerConfig>::iterator it = config_.servers.begin();
         it != config_.servers.end(); ++it) {
        try {
            // Create a new socket
            new_socket = socket_generator_();

            // Bind the socket to the address/port
            int server_id = new_socket->bind(it->listen.first, it->listen.second);

            // Listen for connections
            new_socket->listen();

            // Add the socket to the map
            server_sockets_[server_id] = new_socket;

            // Add the socket to the listener
            listener_->registerEvent(server_id, READABLE);

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
    // Loop forever
    while (true) {
        // Wait for an event
        std::pair<int, InternalEvent> event = listener_->listen();

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
    // Receive the request
    std::pair<HttpRequest, ssize_t> request = receiveRequest(session_id);

    if (request.second == 0) {
        disconnectHandler(session_id);
        return;
    }

    // Handle the request
    HttpResponse response = handleRequest(request.first);

    // Add the response to the clients send queue
    sessions_[session_id]->addSendQueue(response.getMessage());
}

void HttpServer::writableHandler(int session_id) {
    sessions_[session_id]->send(session_id);
}

void HttpServer::errorHandler(int session_id) {
    (void)session_id;
    return;
}

void HttpServer::signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        stop();
    }
}

void HttpServer::connectHandler(int socket_id) {
    // Accept the connection
    Session *session = server_sockets_[socket_id]->accept();

    // Create a new session
    sessions_[session->getSockFd()] = session;

    // Add the session to the listener
    listener_->registerEvent(session->getSockFd(), READABLE); /** @todo event flags */
}

void HttpServer::disconnectHandler(int session_id) {
    // Remove the session from the listener
    listener_->unregisterEvent(session_id);

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

void HttpServer::readRoot(HttpResponse &response, std::string &root, std::string &uri) {
    std::ifstream in(root + uri + "index.html");
    if (in) {
        std::stringstream buffer;
        buffer << in.rdbuf();
        response.body_ = buffer.str();
        in.close();
    } else {
        response.status_ = NOT_FOUND;
    }
}

void HttpServer::buildBody(HttpRequest &request, HttpResponse &response,
                           const ServerConfig &server) {
    const LocationConfig *location = NULL;
    for (std::map<std::string, LocationConfig>::const_iterator it = server.locations.begin();
         it != server.locations.end(); ++it) {
        if (it->first.compare(0, it->first.size(), request.uri_) == 0) {
            location = &(it->second);
        }
    }

    if (location) {
        std::string root = location->root.size() > 0 ? location->root : server.root;
        readRoot(response, root, request.uri_);
    } else {
        response.status_ = NOT_FOUND;
    }
}

bool HttpServer::validateHost(HttpRequest &request, HttpResponse &response) {
    std::string requestHost = request.headers_["Host"];  // Check if the host is valid

    for (std::vector<ServerConfig>::const_iterator it = config_.servers.begin();
         it != config_.servers.end(); ++it) {
        std::string serverHost = it->listen.first + ":" + std::to_string(it->listen.second);
        if (requestHost == serverHost) {
            buildBody(request, response, *it);
            return true;
        }
    }
    return false;
}

HttpResponse HttpServer::handleRequest(HttpRequest request) {
    HttpResponse response;

    response.version_ = HTTP_VERSION;
    response.server_  = "webserv/0.1";

    if (request.version_ != "HTTP/1.1") {
        response.status_ = IM_A_TEAPOT;
    } else if (!validateHost(request, response)) {
        response.status_ = BAD_GATEWAY;
    } else if (request.method_ == GET && request.uri_ == "/") {
        response.status_                  = OK;
        response.headers_["Content-Type"] = "text/html";
    } else {
        response.status_                  = NOT_FOUND;
        response.server_                  = "webserv/0.1";
        response.headers_["Content-Type"] = "text/html";
        response.body_                    = "<html><body><h1>404 Not Found</h1></body></html>";
    }

    return response;
}
