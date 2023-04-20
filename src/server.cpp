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

Server::~Server() {
}

HttpServer::HttpServer(const ServerConfig &config) {
    socket_ = new TcpSocket;
    config_ = config;
}

HttpServer::~HttpServer() throw() {
    delete socket_;
}

void HttpServer::start() {
}

void HttpServer::stop() {
}

void HttpServer::createSocket() {
    /** @todo send in actual addr */
    socket_->bind(config_.listen.second, 0);
    socket_->listen();
}

Request HttpServer::receiveRequest() {
    Request request;

    /** @todo should be client_id not 0 */
    string buffer = socket_->recv(0);

    // start-line
    request.method = buffer.substr(0, buffer.find(' '));
    buffer.erase(0, buffer.find(' ') + 1);
    request.uri = buffer.substr(0, buffer.find(' '));
    buffer.erase(0, buffer.find(' ') + 1);
    request.version = buffer.substr(0, buffer.find(CRLF));
    buffer.erase(0, buffer.find(CRLF) + 2);

    // body
    request.body = buffer.substr(buffer.find("\r\n\r\n") + 4);
    buffer.erase(buffer.find("\r\n\r\n") + 2);

    // headers
    while (buffer.find(CRLF) != string::npos) {
        string key = buffer.substr(0, buffer.find(':'));
        buffer.erase(0, buffer.find(':') + 2);
        string value = buffer.substr(0, buffer.find(CRLF));
        buffer.erase(0, buffer.find(CRLF) + 2);
        request.headers[key] = value;
    }

    return request;
}

Response HttpServer::handleRequest(Request request) {
    /** @todo implement */
    Response response;

    if (request.method == "GET" && request.uri == "/") {
        response.version                 = "HTTP/1.1";
        response.status                  = "200 OK";
        response.server                  = "webserv/0.1";
        response.headers["Content-Type"] = "text/html";
        response.body = "<html><body><h1>Hello World!</h1></body></html>";
    } else {
        response.version                 = "HTTP/1.1";
        response.status                  = "404 Not Found";
        response.server                  = "webserv/0.1";
        response.headers["Content-Type"] = "text/html";
        response.body = "<html><body><h1>404 Not Found</h1></body></html>";
    }

    return response;
}

void HttpServer::sendResponse(Response response) {
    string buffer;

    // status-line
    buffer.append(response.version + " ");
    buffer.append(response.status + " ");
    buffer.append(response.server + CRLF);

    // headers
    for (map<string, string>::iterator it = response.headers.begin();
         it != response.headers.end(); ++it) {
        buffer.append(it->first + ": " + it->second + CRLF);
    }

    // body
    buffer.append(CRLF + response.body);

    /** @todo should be client_id not 0 */
    socket_->send(0, buffer);
}
