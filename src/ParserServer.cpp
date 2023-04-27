/**
 * @file ParserServer.cpp
 * @brief Class implementation for parsing the config file
 *
 * This file contains the class implementation for parsing the config file.
 *
 * @note This code is for educational purposes only and should not be used in
 * production environments without extensive testing and modification.
 *
 * @version 0.1
 * @date 2021-04-19
 * @authors
 *  - Francis L.
 *  - Marc-André L.
 *  - Cole H.
 */

#include "Parser.hpp"

using std::atoi;
using std::logic_error;

int Parser::retrievePort(string num) {
    for (size_t i = 0; i < num.length(); ++i) {
        if (!isdigit(num[i])) {
            throw logic_error("Error: invalid port number for listen: " + num);
        }
    }
    return (atoi(num.c_str()));
}

bool Parser::setListen() {
    validateFirstToken("server_name");
    string num = *it;
    if (num.find(":") == num.npos) {
        int port                           = retrievePort(num);
        (httpConfig.servers.back()).listen = std::make_pair("", port);
    } else {
        string address                     = num.substr(0, num.find(":"));
        num                                = num.substr(num.find(":") + 1);
        int port                           = retrievePort(num);
        (httpConfig.servers.back()).listen = std::make_pair(address, port);
    }
    validateLastToken("listen");
    return (true);
}

bool Parser::setServerName() {
    validateFirstToken("server_name");
    (httpConfig.servers.back()).server_name = *it;
    std::cout << "Server_name: " << *it << std::endl;
    validateLastToken("server_name");
    return (true);
}

bool Parser::setAccessLog() {
    validateFirstToken("access_log");
    std::cout << "Access_log: ";
    while (*it != ";") {
        std::cout << *it << " ";
        ++it;
    }
    std::cout << std::endl;
    return true;
}

bool Parser::setRoot() {
    std::cout << "Root: " << *(++it) << std::endl;
    ++it;
    return (true);
}

bool Parser::setServerSetting() {
    std::cout << "Server: ";
    string List[] = {"listen", "server_name", "access_log", "root", "location"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case LISTEN:
            return setListen();
        case SERVER_NAME:
            return setServerName();
        case ACCESS_LOG:
            return setAccessLog();
        case ROOT:
            return setRoot();
        case LOCATION:
            return setLocationSetting();
        default:
            throw std::invalid_argument("Invalid setting in server context: " +
                                        *it);
    }
    return (true);
}

bool Parser::setServerContext() {
    if (context.back() != 2) {
        throw std::logic_error(
            "Server context needs to be inside http context.");
    }
    httpConfig.servers.push_back(ServerConfig());
    context.push_back(SERVER);
    ++it;
    return true;
}