#pragma once

#include <map>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <limits>
#include <cstdio>

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

    HttpServer(HttpConfig config, SocketGenerator socket_generator = tcp_socket_generator);
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
    bool buildResponse(HttpRequest &, HttpResponse &, ServerConfig &);
    bool getMethod(HttpRequest &, HttpResponse &, ServerConfig &, LocationConfig *);
    bool postMethod(HttpRequest &, HttpResponse &, ServerConfig &, LocationConfig *);
    bool deleteMethod(HttpRequest &, HttpResponse &, ServerConfig &, LocationConfig *);
    bool readFileToBody(HttpResponse &, std::string &, LocationConfig *);
    bool buildNotFound(HttpRequest &, HttpResponse &, ServerConfig &, LocationConfig *);
    bool buildBadRequestBody(HttpResponse &);
    bool isRedirect(HttpRequest &, HttpResponse &, std::pair<int, std::string> &);
    bool validateHost(HttpRequest &, HttpResponse &);
    bool validateRequestBody(HttpRequest &, ServerConfig &, LocationConfig *);
    bool readRoot(HttpRequest &, HttpResponse &, ServerConfig *, LocationConfig *);
    bool checkUriForExtension(std::string &uri, LocationConfig *location) const;
    void handleForbidden(HttpResponse &response, LocationConfig *location, ServerConfig &server);
    void handleIndexFile(HttpRequest &request, HttpResponse &response, LocationConfig *location, ServerConfig &server);
    bool checkIfDirectoryRequest(HttpRequest &request, LocationConfig *location, ServerConfig &server);
    bool checkForIndexFile(HttpRequest &request, LocationConfig *location, ServerConfig &server);
    void generateDirectoryListing(HttpRequest &request, HttpResponse &response, LocationConfig *location, ServerConfig &server);
    std::vector<std::pair<unsigned char, std::string> > returnFiles(HttpRequest &request, LocationConfig *location, ServerConfig &server);
    bool hasTrailingSlash(HttpRequest &request) const;
    void addTrailingSlash(HttpRequest &request, HttpResponse &response);
    std::string getUploadDirectory(ServerConfig &server, LocationConfig *location);
    bool deleteFile(ServerConfig &server, LocationConfig *location, const std::string &filename);
    void uploadsFileList(ServerConfig &server, LocationConfig *location, std::stringstream &fileList);
    std::string generateUniqueFileName(ServerConfig &server, LocationConfig *location, std::string &originalFileName);
    bool displayFile(HttpRequest& request, HttpResponse& response, ServerConfig &server, LocationConfig *location);

    std::string trimHost(const std::string &uri, ServerConfig &server);

   private:
    SocketGenerator          socket_generator_; /**< Function ptr to socket generator */
    std::map<int, Socket *>  server_sockets_;   /**< Map of server IDs to sockets */
    std::map<int, Session *> sessions_;         /**< Map of session IDs to sessions */
    KqueueEventListener      listener_;         /**< Event listener for the server */
    HttpConfig               config_;           /**< Configuration for the server */
};
