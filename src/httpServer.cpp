/**
 * @file httpServer.cpp
 * @author Francis L.
 * @author Marc-André L.
 * @author Cole H.
 * @version 0.1
 * @date 2023-04-19
 * @brief
 */

#include "httpServer.hpp"

/**
 * @brief Constructor
 *
 * @param config Configuration for the server
 */
HttpServer::HttpServer(ServerConfig config) : socket_(), config_(config) {}

/**
 * @brief Destructor
 */
HttpServer::~HttpServer() throw() {}

/**
 * @brief Start the server
 */
void HttpServer::start() {}

/**
 * @brief Read a request from the socket
 *
 * @return Request
 *
 * @todo How should we handle the port?
 */
Request readRequest(TcpSocket socket, int port) {
    Request request;

    string buffer = socket.recv(port);

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

/**
 * @brief Write a response to the socket
 *
 * @param response Response to write
 *
 * @todo How How should we handle the port?
 */
void writeResponse(int port, TcpSocket socket, Response response) {
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

    socket.send(port, buffer);
}
