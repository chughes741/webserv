#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "Exception.hpp"

#define WORKER_CONNECTIONS 0
#define USE 1
#define MULTI_ACCEPT 2
#define ACCEPT_MUTEX_DELAY 3
#define DEBUG_CONNECTION 4
#define USE_POLL 5
#define DEFERRED_ACCEPT 6

class Events {
   public:
    // Default constructor (Required)
    Events();
    // Copy constructor (Required)
    Events(const Events &copy);
    // Constructors
    // Default Destructor (Required)
    ~Events();
    // Destructors
    // Copy assignment operator (Required)
    Events &operator=(const Events &);
    // Operators
    // Swap function
    // Setters
    void setWorkerConnections(std::vector<std::string>::iterator &);
    void setUse(std::vector<std::string>::iterator &);
    // Getters
    int         getWorkerConnections() const { return worker_connections; }
    std::string getUse() const { return use; }
    // Other functions
    bool isSetting(std::string);
    void setSetting(std::string &, std::vector<std::string>::iterator &);

   protected:
   private:
    std::vector<std::string> settings;
    /*  Specifies the maximum number of connections that each worker process can
     * handle simultaneously. */
    int worker_connections;
    // /* Specifies the event module to be used. The available options are
    // "epoll", "kqueue", "devpoll", "poll", and "select". */
    std::string use;  // epoll, kqueue, devpoll, poll, select
    // /* Enables or disables the ability of a worker process to accept multiple
    // connections at once. */ bool multi_accept; // On or off
    // /* Enables or disables the accept mutex, which prevents multiple worker
    // processes from trying to accept connections at the same time. */ bool
    // accept_mutex_delay; //time in second or ms or 0
    // /* TO IGNORE Specifies the IP address and port number of a client that
    // should be allowed to send debugging commands to nginx. This directive is
    // only available if nginx was compiled with the --with-debug configuration
    // option. */ std::string debug_connection; // ip address with or not the
    // port. if compiled with -- debug
    // /* Enables or disables the use of the poll system call instead of select
    // for I/O multiplexing */ bool use_poll; // Yes or no
    // /* Enables or disables the ability of a worker process to defer accepting
    // a connection until data has been received. */ bool deferred_accept; // on
    // or off
};

/** HTTP methods */
#define POST "POST"
#define GET "GET"
#define DELETE "DELETE"

/** HTTP Status codes */
#define OK "200 OK"
#define CREATED "201 Created"
#define ACCEPTED "202 Accepted"
#define NO_CONTENT "204 No Content"
#define MOVED_PERMANENTLY "301 Moved Permanently"
#define FOUND "302 Found"
#define NOT_MODIFIED "304 Not Modified"
#define BAD_REQUEST "400 Bad Request"
#define NOT_FOUND "404 Not Found"
#define METHOD_NOT_ALLOWED "405 Method Not Allowed"
#define IM_A_TEAPOT "418 I'm a teapot"
#define INTERNAL_SERVER_ERROR "500 Internal Server Error"
#define BAD_GATEWAY "502 Bad Gateway"


#define HTTP_VERSION "HTTP/1.1"

/**
 * @brief Represents an HTTP request
 */
struct Request {
    std::string method;          /**< HTTP method (GET, POST, etc.) */
    std::string uri;             /**< Request URI */
    std::string version;         /**< HTTP version */

    std::string host;            /**< Value of the Host header */
    std::string connection;      /**< Value of the Connection header */
    std::string accept;          /**< Value of the Accept header */
    std::string accept_encoding; /**< Value of the Accept-Encoding header */
    std::string accept_language; /**< Value of the Accept-Language header */
    std::string user_agent;      /**< Value of the User-Agent header */
    std::string content_length;  /**< Value of the Content-Length header */
    std::string content_type;    /**< Value of the Content-Type header */

    std::string body;            /**< Request body (if any) */
};

/**
 * @brief Represents an HTTP response
 */
struct Response {
    std::string version;        /**< HTTP version */
    std::string status;         /**< HTTP status code and message */

    std::string server;         /**< Value of the Server header */
    std::string date;           /**< Value of the Date header */
    std::string content_type;   /**< Value of the Content-Type header */
    std::string content_length; /**< Value of the Content-Length header */
    std::string connection;     /**< Value of the Connection header */

    std::string body;           /**< Response body (if any) */
};
