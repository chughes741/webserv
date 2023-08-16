#pragma once

#include <map>
#include <string>

#include "webserv.hpp"

/** HTTP headers */
#define HTTP_VERSION "HTTP/1.1"

/** HTTP methods */
enum HttpMethod {
    GET,
    POST,
    DELETE
};

/** HTTP status codes */
enum HttpStatus {
    OK                    = 200,
    CREATED               = 201,
    ACCEPTED              = 202,
    NO_CONTENT            = 204,
    MOVED_PERMANENTLY     = 301,
    FOUND                 = 302,
    NOT_MODIFIED          = 304,
    BAD_REQUEST           = 400,
    NOT_FOUND             = 404,
    METHOD_NOT_ALLOWED    = 405,
    IM_A_TEAPOT           = 418,
    INTERNAL_SERVER_ERROR = 500,
    BAD_GATEWAY           = 502
};

/** Represents an HTTP request */
struct HttpRequest {
    HttpMethod                         method;  /**< HTTP method (GET, POST, etc.) */
    std::string                        uri;     /**< Request URI */
    std::string                        version; /**< HTTP version */
    std::map<std::string, std::string> headers; /**< Other headers */
    std::string                        body;    /**< Request body (if any) */
};

/** Represents an HTTP response */
struct HttpResponse {
    std::string                        version; /**< HTTP version */
    HttpStatus                         status;  /**< HTTP status code and message */
    std::string                        server;  /**< Value of the Server header */
    std::map<std::string, std::string> headers; /**< Other headers */
    std::string                        body;    /**< Response body (if any) */
};
