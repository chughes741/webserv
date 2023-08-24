#pragma once

#include <map>
#include <string>

/** HTTP headers */
#define HTTP_VERSION "HTTP/1.1"
#define CRLF         "\r\n"

/** HTTP methods */
enum HttpMethod {
    UNKNOWN,
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
class HttpRequest {
   public:
    HttpRequest(const std::string &request);

   private:
    std::string consumeNextToken(std::string &buffer, const std::string &delimiter);

   public:
    HttpMethod                               method_;    /**< HTTP method (GET, POST, etc.) */
    std::string                              uri_;       /**< Request URI */
    std::string                              version_;   /**< HTTP version */
    std::map<std::string, std::string>       headers_;   /**< Other headers */
    std::string                              body_;      /**< Request body (if any) */
    static std::map<std::string, HttpMethod> methodMap_; /**< Map of HTTP methods */
};

/** Represents an HTTP response */
class HttpResponse {
   public:
    std::string getMessage() const;

   public:
    std::string                              version_;   /**< HTTP version */
    HttpStatus                               status_;    /**< HTTP status code and message */
    std::string                              server_;    /**< Value of the Server header */
    std::map<std::string, std::string>       headers_;   /**< Other headers */
    std::string                              body_;      /**< Response body (if any) */
    static std::map<HttpStatus, std::string> statusMap_; /**< Map of HTTP status codes */
};
