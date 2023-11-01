#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <exception>
#include <iostream>
#include <stdexcept>
#include "logging.hpp"
#include "http.hpp"
#include "config.hpp"
// #include "../include/config.hpp"

enum exceptionType {
    Internal,
    Permission,
    Access
};

class Cgi {
public:
    Cgi(HttpRequest request, HttpMethod method, std::string cgiPath, HttpConfig config);
    ~Cgi();
    void performCgi();

private: //private methods
    void checkForScript();
    void handleError(exceptionType type);
    void setEnv();
    void handlePipe();
    void performCgiGet();
    void performCgiPost();

private: //member variables
    HttpRequest request;
    HttpMethod method;
    std::string pathToScript;
    HttpConfig config;
    char **envp;
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

};