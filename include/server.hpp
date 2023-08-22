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

#include "events.hpp"
#include "webserv.hpp"

/** @todo figure out why this aren't working with includes from webserv.hpp */
class Socket;
class Session;
Socket* tcp_socket_generator();

/**
 * @brief Base class for servers
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

    virtual HttpRequest  receiveRequest(int session_id)      = 0;
    virtual HttpResponse handleRequest(HttpRequest request)  = 0;
    virtual void         sendResponse(int session_id, HttpResponse response) = 0;

   protected:
    SocketGenerator                   socket_generator_; /**< Function ptr to socket generator */
    std::map<int, Socket*>            server_sockets_;   /**< Map of server IDs to sockets */
    std::map<int, Session*>           sessions_;         /**< Map of session IDs to sessions */
    EventListener*                    listener_;         /**< Event listener for the server */
    HttpConfig                        config_;           /**< Configuration for the server */
    std::map<std::string, HttpMethod> http_methods_;     /**< HTTP methods */
    std::map<HttpStatus, std::string> http_status_;      /**< HTTP status messages */
};

/**
 * @brief Represents an HTTP server
 */
class HttpServer : public Server {
   public:
    HttpServer(HttpConfig config, EventListener* listener,
               SocketGenerator socket_generator = tcp_socket_generator);
    ~HttpServer();

   private:
    // HttpServer(const HttpServer& other) {}
    // HttpServer& operator=(const HttpServer& other) {}

   public:
    void start(bool run_server = true);
    void stop();

   private:
    void run();

    void readableHandler(int server_id);
    void writableHandler(int server_id);
    void errorHandler(int server_id);
    void connectHandler(int socket_id);
    void disconnectHandler(int session_id);

    HttpRequest  receiveRequest(int session_id);
    HttpResponse handleRequest(HttpRequest request);
    void         sendResponse(int session_id, HttpResponse response);
    void         buildBody(HttpResponse &response);
    bool         validateHost(HttpRequest &request);
};

