#pragma once

#include "webserv.hpp"

#define WORKER_CONNECTIONS 0
#define USE 1
#define MULTI_ACCEPT 2
#define ACCEPT_MUTEX_DELAY 3
#define DEBUG_CONNECTION 4
#define USE_POLL 5
#define DEFERRED_ACCEPT 6

/** HTTP methods */
#define POST 0x01
#define GET 0x02
#define DELETE 0x04

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

/** HTTP headers */
#define HTTP_VERSION "HTTP/1.1"

using std::map;
using std::string;

/**
 * @brief Represents an HTTP request
 */
struct Request {
    string              method;  /**< HTTP method (GET, POST, etc.) */
    string              uri;     /**< Request URI */
    string              version; /**< HTTP version */
    map<string, string> headers; /**< Other headers */
    string              body;    /**< Request body (if any) */
};

/**
 * @brief Represents an HTTP response
 */
struct Response {
    string              version; /**< HTTP version */
    string              status;  /**< HTTP status code and message */
    string              server;  /**< Value of the Server header */
    map<string, string> headers; /**< Other headers */
    string              body;    /**< Response body (if any) */
};

Request readRequest(int);
void    writeResponse(int, Response);
