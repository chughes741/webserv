/**
 * @file server.hpp
 * @brief Declares classes for creating web servers that can handle HTTP
 * requests.
 *
 * This file contains the declaration of the Server and HttpServer classes.
 * The Server class is an abstract base class that provides an interface for
 * creating servers. The HttpServer class inherits from the Server class and
 * implements an HTTP server that can receive and send HTTP requests and
 * responses.
 *
 * The HttpServer class can be configured with a ServerConfig object that
 * specifies the IP address and port number on which to listen for incoming
 * connections.
 *
 * The HttpServer class currently supports only the GET method for serving
 * static files. It can serve a default HTML file if the root URI is requested,
 * or it can return a 404 Not Found error for any other URI.
 *
 * To use the HttpServer class, you must create an instance of it with a
 * ServerConfig object, call the start() method to begin listening for
 * connections, and then call the receiveRequest() method in a loop to receive
 * incoming HTTP requests. Once you have received a request, you can use the
 * handleRequest() method to generate a response, and then send the response
 * with the sendResponse() method.
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

#pragma once

#include "webserv.hpp"

using std::map;
using std::string;

/** @todo figure out why this aren't working with includes from webserv.hpp */
class Socket;
class Session;
Socket* tcp_socket_generator();

/**
 * @brief Base class for servers
 * @pure
 */
class Server {
   public:
    typedef Socket* (*SocketGenerator)(void);

    Server(HttpConfig config, EventListener* listener, SocketGenerator socket_generator);
    virtual ~Server() = 0;

    virtual void start(bool) = 0;
    virtual void stop()      = 0;

   protected:
    virtual void run() = 0;

    virtual Request  receiveRequest()                = 0;
    virtual Response handleRequest(Request request)  = 0;
    virtual void     sendResponse(Response response) = 0;

   protected:
    SocketGenerator    socket_generator_; /**< Function ptr to socket generator */
    map<int, Socket*>  server_sockets_;   /**< Map of server IDs to sockets */
    map<int, Session*> sessions_;         /**< Map of session IDs to sessions */
    EventListener*     listener_;         /**< Event listener for the server */
    HttpConfig         config_;           /**< Configuration for the server */
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
    HttpServer(HttpConfig config, EventListener* listener,
               SocketGenerator socket_generator = tcp_socket_generator);
    ~HttpServer();

   private:
    // HttpServer(const HttpServer& other) {}
    // HttpServer& operator=(const HttpServer& other) {}

   public:
    /**
     * @brief Start the server
     */
    void start(bool run_server = true);

    /**
     * @brief Stop the server
     */
    void stop();

   private:
    /**
     * @brief Run the server
     */
    void run();

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
