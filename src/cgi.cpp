#include "../include/cgi.hpp"

Cgi::Cgi(std::string cgiPath): envp(nullptr), pathToScript(cgiPath) {
    try
    {
        this->checkForScript();
        this->setEnv();
        this->handlePipe();
        this->performCgi();
    }
    catch(const Cgi::InternalServerError& e) {
        Logger::instance().log(e.what());
        this->handleError(Internal);
    }
    catch(const Cgi::ForbiddenFile& e) {
        Logger::instance().log(e.what());
        this->handleError(Permission);
    }
    catch(const Cgi::InvalidPath& e) {
        Logger::instance().log(e.what());
        this->handleError(Access);
    }
    catch(const std::exception& e) {
        Logger::instance().log(e.what());
        this->handleError(Internal);
    }
    

}

Cgi::~Cgi() { }

void Cgi::performCgi() {

}

void Cgi::setEnv() {
    (void) this->envp;
}

void Cgi::handlePipe() {

}

void Cgi::checkForScript() {
    
}

void Cgi::handleError(exceptionType type) {
    switch(type) {
        case (Internal):

            break;
        case (Permission):

            break;
        case (Access):

            break;
        default:
            throw std::runtime_error("Something messed up happened");
            break;
    }

}

const char *Cgi::InternalServerError::what() const throw() {
    return "Internal server error exception";
}

const char *Cgi::ForbiddenFile::what() const throw() {
    return "Access forbidden";
}

const char *Cgi::InvalidPath::what() const throw() {
    return "Ressource does not exist";
}