#include "http.hpp"

std::map<std::string, HttpMethod> initMethodMap() {
    std::map<std::string, HttpMethod> methodMap;
    methodMap["GET"]    = GET;
    methodMap["POST"]   = POST;
    methodMap["DELETE"] = DELETE;
    return methodMap;
}

std::map<std::string, HttpMethod> HttpRequest::methodMap_ = initMethodMap();

std::string HttpRequest::consumeNextToken(std::string &buffer, const std::string &delimiter) {
    if (buffer.find(delimiter) == std::string::npos) {
        return "";
    }

    std::string token = buffer.substr(0, buffer.find(delimiter));
    buffer.erase(0, buffer.find(delimiter) + delimiter.size());
    return token;
}

HttpRequest::HttpRequest(const std::string &request) {
    std::string buffer = request;

    std::string method = consumeNextToken(buffer, " ");

    method_ = methodMap_.find(method) != methodMap_.end() ? methodMap_[method] : UNKNOWN;

    uri_     = consumeNextToken(buffer, " ");
    version_ = consumeNextToken(buffer, CRLF);

    std::string headers = consumeNextToken(buffer, "\r\n\r\n");
    while (headers.find(CRLF) != std::string::npos) {
        std::string key = consumeNextToken(headers, ":");
        consumeNextToken(headers, " ");
        std::string value = consumeNextToken(headers, CRLF);
        headers_[key]     = value;
    }

    body_ = buffer;
}

std::map<HttpStatus, std::string> initStatusMap() {
    std::map<HttpStatus, std::string> statusMap;
    statusMap[OK]                    = "200 OK";
    statusMap[CREATED]               = "201 Created";
    statusMap[ACCEPTED]              = "202 Accepted";
    statusMap[NO_CONTENT]            = "204 No Content";
    statusMap[MOVED_PERMANENTLY]     = "301 Moved Permanently";
    statusMap[FOUND]                 = "302 Found";
    statusMap[NOT_MODIFIED]          = "304 Not Modified";
    statusMap[BAD_REQUEST]           = "400 Bad Request";
    statusMap[NOT_FOUND]             = "404 Not Found";
    statusMap[METHOD_NOT_ALLOWED]    = "405 Method Not Allowed";
    statusMap[IM_A_TEAPOT]           = "418 I'm a teapot";
    statusMap[INTERNAL_SERVER_ERROR] = "500 Internal Server Error";
    statusMap[BAD_GATEWAY]           = "502 Bad Gateway";
    return statusMap;
}

std::map<HttpStatus, std::string> HttpResponse::statusMap_ = initStatusMap();

std::string HttpResponse::getMessage() const {
    std::string buffer;

    // status-line
    buffer.append(version_ + " ");

    // Find the status string from the map using the enum value.
    buffer.append(statusMap_[status_] + CRLF);

    // Append the server name
    buffer.append(server_ + CRLF);

    // headers
    for (std::map<std::string, std::string>::const_iterator it = headers_.begin();
         it != headers_.end(); ++it) {
        buffer.append(it->first + ": " + it->second + CRLF);
    }

    // body
    buffer.append(CRLF + body_);

    return buffer;
}

std::string HttpRequest::printRequest() const {
    std::ostringstream oss;

    oss << "Method: " << method_ << "\n";
    oss << "URI: " << uri_ << "\n";
    oss << "Version: " << version_ << "\n";
    oss << "Headers:\n";
    for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        oss << "  " << it->first << ": " << it->second << "\n";
    }
    if (!body_.empty()) {
        oss << "Body:\n" << body_ << "\n";
    }
    return oss.str();
}
