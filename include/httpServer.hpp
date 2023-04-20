/**
 * @file httpServer.hpp
 * @author Francis L.
 * @author Marc-André L.
 * @author Cole H.
 * @version 0.1
 * @date 2023-04-19
 * @brief Class for handling HTTP requests
 */

#pragma once

#include "webserv.hpp"

using std::string;

/**
 * @brief Represents an HTTP server
 */
class HttpServer {
   public:
    HttpServer(ServerConfig config);
    ~HttpServer() throw();

   private:
    HttpServer(const HttpServer& other) {}
    HttpServer& operator=(const HttpServer& other) {}

   public:
    void start();

   private:
    TcpSocket    socket_; /**< Socket used by the server */
    ServerConfig config_; /**< Configuration for the server */
    EventsConfig events_; /**< Configuration for server events */
};
