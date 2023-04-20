/**
 * @file server.hpp
 * @author Francis L.
 * @author Marc-André L.
 * @author Cole H.
 * @version 0.1
 * @date 2023-04-19
 * @brief Classes for creating web servers
 */

#pragma once

#include "webserv.hpp"

using std::string;

/**
 * @brief Base class for servers
 * @pure
 */
class Server {
   public:
    virtual ~Server() = 0;

    virtual void start()        = 0;
    virtual void stop()         = 0;
    virtual void createSocket() = 0;

   protected:
    virtual Request  readRequest()                   = 0;
    virtual Response handleRequest(Request request)  = 0;
    virtual void     sendResponse(Response response) = 0;

   protected:
    Socket*      socket_; /**< Socket used by the server */
    ServerConfig config_; /**< Configuration for the server */
};

/**
 * @brief Represents an HTTP server
 */
class HttpServer : public Server {
   public:
    /**
     * @brief Constructor
     *
     * @param config Configuration for the server
     */
    HttpServer(const ServerConfig& config);
    ~HttpServer() throw();

   private:
    // HttpServer(const HttpServer& other) {}
    // HttpServer& operator=(const HttpServer& other) {}

   public:
    /**
     * @brief Start the server
     */
    void start();

    /**
     * @brief Stop the server
     */
    void stop();

    /**
     * @brief Create a server socket
     */
    void createSocket();

   private:
    /**
     * @brief Read a request from the socket
     *
     * @return Request
     */

    Request receiveRequest();

    /**
     * @brief Handle a request
     *
     * @param request Request to handle
     * @return Response
     */
    Response handleRequest(Request request);

    /**
     * @brief Write a response to the socket
     *
     * @param response Response to write
     */
    void sendResponse(Response response);
};
