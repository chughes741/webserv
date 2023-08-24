#pragma once

#include <map>
#include <string>

#include "config.hpp"
#include "http.hpp"
#include "events.hpp"
#include "socket.hpp"

class Socket;
class Session;
Socket *tcp_socket_generator();

// HTTP server
class HttpServer {
   public:
    typedef Socket *(*SocketGenerator)(void);

    HttpServer(HttpConfig config, EventListener *listener = new KqueueEventListener(),
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
    void signalHandler(int signal);
    void connectHandler(int socket_id);
    void disconnectHandler(int session_id);

    std::pair<HttpRequest, ssize_t> receiveRequest(int session_id);
    HttpResponse                    handleRequest(HttpRequest request);
    void buildBody(HttpRequest &request, HttpResponse &response, const ServerConfig &config);
    bool validateHost(HttpRequest &request, HttpResponse &response);
    void readRoot(HttpResponse &response, std::string &root, std::string &uri);

   private:
    SocketGenerator          socket_generator_; /**< Function ptr to socket generator */
    std::map<int, Socket *>  server_sockets_;   /**< Map of server IDs to sockets */
    std::map<int, Session *> sessions_;         /**< Map of session IDs to sessions */
    EventListener           *listener_;         /**< Event listener for the server */
    HttpConfig               config_;           /**< Configuration for the server */
};
