#include "Parser.hpp"

void Parser::setFastCGI(string &uri) {
    string path = *(++it);
    std::cout << "Fastcgi: " << path << std::endl;
    if (*(++it) != ";") {
        throw std::logic_error("Invalid syntax for location after fastcgi: " +
                               *it);
    }
    (httpConfig.servers.back()).locations[uri].cgi_path    = path;
    (httpConfig.servers.back()).locations[uri].cgi_enabled = true;
}

void Parser::setPath(string &uri) {
    string path = *(++it);
    std::cout << "Path: " << path << std::endl;
    if (*(++it) != ";") {
        throw std::logic_error("Invalid syntax for location after path: " +
                               *it);
    }
    (httpConfig.servers.back()).locations[uri].root = path;
}

string Parser::setLocationUri() {
    string path = *(++it);
    if (*(++it) != "{") {
        throw std::logic_error("Invalid syntax for location: " + *it);
    }
    (httpConfig.servers.back()).locations[path] = LocationConfig();
    ++it;
    return path;
}

/**
 * @brief 		Set http setting
 */
bool Parser::setLocationSetting() {
    string List[] = {"path:", "fastcgi:"};
    string uri    = setLocationUri();
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case PATH:
            setPath(uri);
            break;
        case FASTCGI:
            setFastCGI(uri);
            break;
        default:
            throw std::logic_error("Invalid setting for location: " + *it);
    }
    if (*(++it) != "}") {
        throw std::logic_error("Invalid syntax for location after setting: " +
                               *it);
    }
    return true;
}
