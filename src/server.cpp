
/**
 * @file server.cpp
 * @brief Defines classes for creating web servers that can handle HTTP
 * requests.
 *
 * This file contains the implementation of the Server and HttpServer classes.
 * The Server class is an abstract base class that provides an interface for
 * creating servers. The HttpServer class inherits from the Server class and
 * implements an HTTP server that can receive and send HTTP requests and
 * responses.
 *
 * @note This code is for educational purposes only and should not be used in
 * production environments without extensive testing and modification.
 *
 * @version 0.1
 * @date 2023-04-19
 * @authors
 *   - Francis L.
 *   - Marc-André L.
 *   - Cole H.
 */

#include "server.hpp"

extern HttpConfig httpConfig;

Server::Server(HttpConfig config, EventListener* listener, SocketGenerator socket_generator)
    : socket_generator_(socket_generator), listener_(listener), config_(config) {
    http_methods_["GET"]    = GET;
    http_methods_["POST"]   = POST;
    http_methods_["DELETE"] = DELETE;

    http_status_[OK]                    = "200 OK";
    http_status_[CREATED]               = "201 Created";
    http_status_[ACCEPTED]              = "202 Accepted";
    http_status_[NO_CONTENT]            = "204 No Content";
    http_status_[MOVED_PERMANENTLY]     = "301 Moved Permanently";
    http_status_[FOUND]                 = "302 Found";
    http_status_[NOT_MODIFIED]          = "304 Not Modified";
    http_status_[BAD_REQUEST]           = "400 Bad Request";
    http_status_[NOT_FOUND]             = "404 Not Found";
    http_status_[METHOD_NOT_ALLOWED]    = "405 Method Not Allowed";
    http_status_[IM_A_TEAPOT]           = "418 I'm a teapot";
    http_status_[INTERNAL_SERVER_ERROR] = "500 Internal Server Error";
    http_status_[BAD_GATEWAY]           = "502 Bad Gateway";
}

Server::~Server() {}

HttpServer::HttpServer(HttpConfig httpConfig, EventListener* listener,
                       SocketGenerator socket_generator)
    : Server(httpConfig, listener, socket_generator) {}

HttpServer::~HttpServer() {}

void HttpServer::start(bool run_server) {
    // Create a socket for each server in the config
    Socket* new_socket;
    for (std::vector<ServerConfig>::iterator it = config_.servers.begin();
         it != config_.servers.end(); ++it) {
        try {
            // Create a new socket
            new_socket = socket_generator_();

            // Bind the socket to the address/port
            int server_id = new_socket->bind(it->listen.first, it->listen.second);

            // Listen for connections
            /** @todo poll needs to be called before listen */
            new_socket->listen();

            // Add the socket to the map
            server_sockets_[server_id] = new_socket;

            // Add the socket to the listener
            listener_->registerEvent(server_id, READABLE); /** @todo event flags */
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;

            // Delete the socket, is this safe even if it wasn't constructed?
            delete new_socket;
        }
    }

    // Run the server
    if (run_server == true) run();
}

void HttpServer::stop() {
    // Close all sockets and delete them
    for (std::map<int, Socket*>::iterator it = server_sockets_.begin(); it != server_sockets_.end();
         ++it) {
        try {
            it->second->close();
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
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
        std::pair<int, uint32_t> event = listener_->listen();

        // Handle event
        switch (event.second) {
            case READABLE:
                readableHandler(event.first);
                break;
            case WRITABLE:
                writableHandler(event.first);
                break;
            case ERROR_EVENT:
                errorHandler(event.first);
                break;
            case CONNECT_EVENT:
                connectHandler(event.first);
                break;
        }
    }
}

void HttpServer::readableHandler(int session_id) {
    (void)session_id;
    return;
}

void HttpServer::writableHandler(int session_id) {
    (void)session_id;
    return;
}

void HttpServer::errorHandler(int session_id) {
    (void)session_id;
    return;
}

void HttpServer::connectHandler(int socket_id) {
    // Accept the connection
    Session* session = server_sockets_[socket_id]->accept();

    // Create a new session
    sessions_[session->getSockFd()] = session;

    // Add the session to the listener
    listener_->registerEvent(session->getSockFd(), 0); /** @todo event flags */
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

HttpRequest HttpServer::receiveRequest() {
    HttpRequest request;

    /** @todo should be client_id not 0 */
    std::string buffer = sessions_[0]->recv(0);

    // start-line
    std::string method = buffer.substr(0, buffer.find(' '));
    buffer.erase(0, buffer.find(' ') + 1);

    // Use the map to convert the string method to its corresponding enum
    std::map<std::string, HttpMethod>::iterator it = http_methods_.find(method);
    if (it != http_methods_.end()) {
        request.method = it->second;
    } else {
        throw std::runtime_error("Unknown HTTP method");
    }

    buffer.erase(0, buffer.find(' ') + 1);
    request.uri = buffer.substr(0, buffer.find(' '));
    buffer.erase(0, buffer.find(' ') + 1);
    request.version = buffer.substr(0, buffer.find(CRLF));
    buffer.erase(0, buffer.find(CRLF) + 2);

    // body
    request.body = buffer.substr(buffer.find("\r\n\r\n") + 4);
    buffer.erase(buffer.find("\r\n\r\n") + 2);

    // headers
    while (buffer.find(CRLF) != std::string::npos) {
        std::string key = buffer.substr(0, buffer.find(':'));
        buffer.erase(0, buffer.find(':') + 2);
        std::string value = buffer.substr(0, buffer.find(CRLF));
        buffer.erase(0, buffer.find(CRLF) + 2);
        request.headers[key] = value;
    }

    return request;
}

HttpResponse HttpServer::handleRequest(HttpRequest request) {
    /** @todo implement */
    HttpResponse response;

    if (request.method == GET && request.uri == "/") {
        response.version                 = "HTTP/1.1";
        response.status                  = OK;
        response.server                  = "webserv/0.1";
        response.headers["Content-Type"] = "text/html";
        response.body                    = "<html><body><h1>Hello World!</h1></body></html>";
    } else {
        response.version                 = "HTTP/1.1";
        response.status                  = NOT_FOUND;
        response.server                  = "webserv/0.1";
        response.headers["Content-Type"] = "text/html";
        response.body                    = "<html><body><h1>404 Not Found</h1></body></html>";
    }

    return response;
}

void HttpServer::sendResponse(HttpResponse response) {
    std::string buffer;

    // status-line
    buffer.append(response.version + " ");

    // Find the status string from the map using the enum value.
    std::map<HttpStatus, std::string>::iterator statusIt = http_status_.find(response.status);
    if (statusIt != http_status_.end()) {
        buffer.append(statusIt->second);
    } else {
        buffer.append(http_status_[INTERNAL_SERVER_ERROR]);
    }

    // Append the server name
    buffer.append(response.server + CRLF);

    // headers
    for (std::map<std::string, std::string>::iterator it = response.headers.begin();
         it != response.headers.end(); ++it) {
        buffer.append(it->first + ": " + it->second + CRLF);
    }

    // body
    buffer.append(CRLF + response.body);

    /** @todo should be client_id not 0 */
    sessions_[0]->send(0, buffer);
}
