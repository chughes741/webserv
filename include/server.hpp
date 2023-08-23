#pragma once

#include "events.hpp"
#include "webserv.hpp"

class Socket;
class Session;
Socket *tcp_socket_generator();

// Abstract base class for servers
class Server {
   public:
    typedef Socket *(*SocketGenerator)(void);

    Server(HttpConfig config, EventListener *listener, SocketGenerator socket_generator);
    virtual ~Server() = 0;

    virtual void start(bool) = 0;
    virtual void stop()      = 0;

   protected:
    virtual void run() = 0;

    virtual std::pair<HttpRequest, ssize_t> receiveRequest(int session_id)                      = 0;
    virtual HttpResponse                    handleRequest(HttpRequest request)                  = 0;
    virtual void                            sendResponse(int session_id, HttpResponse response) = 0;

   protected:
    SocketGenerator                   socket_generator_; /**< Function ptr to socket generator */
    std::map<int, Socket *>           server_sockets_;   /**< Map of server IDs to sockets */
    std::map<int, Session *>          sessions_;         /**< Map of session IDs to sessions */
    EventListener                    *listener_;         /**< Event listener for the server */
    HttpConfig                        config_;           /**< Configuration for the server */
    std::map<std::string, HttpMethod> http_methods_;     /**< HTTP methods */
    std::map<HttpStatus, std::string> http_status_;      /**< HTTP status messages */
};

// HTTP server
class HttpServer : public Server {
   public:
    HttpServer(HttpConfig config, EventListener *listener,
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
    void                            sendResponse(int session_id, HttpResponse response);
    void buildBody(HttpRequest &request, HttpResponse &response, const ServerConfig &config);
    bool validateHost(HttpRequest &request, HttpResponse &response);
    void readRoot(HttpResponse &response, std::string &root, std::string &uri);
};
