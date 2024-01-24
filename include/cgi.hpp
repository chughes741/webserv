#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <exception>
#include <iostream>
#include <stdexcept>
#include "logging.hpp"
#include "http.hpp"
#include "config.hpp"

enum exceptionType {
    Internal,
    Access
};

class Cgi {
public:
    Cgi(HttpRequest &request, LocationConfig &location, ServerConfig &config, HttpResponse& response);
    ~Cgi();
    bool exec();

private: //private methods
    bool performCgi();
    void checkForScript();
    void handleError(exceptionType type);
    void setEnv();
    bool performCgiGet();
    bool performCgiPost();
    void extractScript();
    void extractHeaders(std::string scriptOutput);
    void extractBody(std::string scriptOutput);

private: //member variables

    HttpRequest request_;
    LocationConfig location_;
    ServerConfig config_;
    HttpResponse* response_;
    char *envp_[256];
    std::string script_;
    std::string scriptWithPath_;
    std::vector<std::string> meta_variables_;
    class InternalServerError: public std::exception {
    public:
        const char *what() const throw();
    };

    class ForbiddenFile: public std::exception {
    public:
        const char* what() const throw();
    };

    class InvalidPath: public std::exception {
    public:
        const char *what() const throw();
    };

    class UnsupportedMethod: public std::exception {
    public:
        const char *what() const throw();
    };

    class RessourceDoesNotExist: public std::exception {
    public:
        const char *what() const throw();
    };

};