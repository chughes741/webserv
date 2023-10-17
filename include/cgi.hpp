#include <fcntl.h>
#include <string>
#include <exception>
#include <iostream>
#include "logging.hpp"
// #include "../include/config.hpp"

enum exceptionType {
    Internal,
    Permission,
    Access
};

class Cgi {
public:
    Cgi(std::string cgiPath);
    ~Cgi();
    void performCgi();

private: //private methods
    void checkForScript();
    void handleError(exceptionType type);
    void setEnv();
    void handlePipe();

private: //member variables
    char **envp;
    std::string pathToScript;
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
    }



};