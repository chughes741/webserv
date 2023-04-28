/**
 * @file parsing.cpp
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

#include "parsing.hpp"

#define INDEX 0

using std::atoi;
using std::find;
using std::logic_error;

void parseConfig(string config_file, HttpConfig &httpConfig) {
    Parser        parser(httpConfig);
    string        line;
    std::ifstream file(config_file.c_str());

    if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
        throw std::invalid_argument("File not found:  " + config_file);
    }
    while (getline(file, line)) {
        parser.tokenizeConfig(line);
    }
    file.close();
    parser.initSettings();
}

Parser::Parser(HttpConfig &httpConfig) : context(1), httpConfig(httpConfig) {
}

Parser::~Parser(void) {
    tokens.clear();
}

void Parser::tokenizeConfig(string line) {
    while (line.size() > 0) {
        size_t pos = line.find_first_not_of(" \t");
        if (pos == line.npos || line[pos] == '#') {
            break;
        }
        line = line.substr(pos);
        pos  = line.find_first_of("\\#{}; \t\n\0");
        pos == 0 ? pos = 1 : pos;
        string tmp = line.substr(0, pos);
        tokens.push_back(tmp);
        ;
        line = line.substr(tmp.size());
    }
}

void Parser::validateFirstToken(string setting) {
    if (*(++it) == ";") {
        throw std::invalid_argument("Error: missing argument for " + setting);
    }
}

void Parser::validateLastToken(string setting) {
    if (*(++it) != ";") {
        throw std::invalid_argument("Error: too many arguments for " + setting +
                                    " (" + *it + ")");
    }
}

void Parser::initSettings() {
    it = tokens.begin();
    for (; it != tokens.end(); ++it) {
        while (*it == "}" && !context.empty()) {
            context.pop_back();
            ++it;
        }
        if (it == tokens.end()) {
            break;
        }
        setContext();
        if (context.back() == -1) {
            context.pop_back();
        }
        contextSwitchCase(context.back());
    }
    if (context.size() != 1) {
        throw std::exception();
    }
}

bool Parser::contextSwitchCase(int context) {
    switch (context) {
        case GLOBAL:
            return setGlobalSetting();
        case EVENTS:
            return setEventsSetting();
        case HTTP:
            return setHttpSetting();
        case SERVER:
            return setServerSetting();
        default:
            throw std::logic_error("Invalid context: " + *it);
    }
    return true;
}

bool Parser::setContext() {
    string item   = *it;
    string List[] = {"", "events", "http", "server"};
    int    tmp    = getSetting(List, sizeof(List) / sizeof(List[0]));
    if (*(++it) == "{" && tmp != -1) {
        switch (tmp) {
            case EVENTS:
                return setEventsContext();
            case HTTP:
                return setHttpContext();
            case SERVER:
                return setServerContext();
            default:
                throw std::logic_error("Invalid context: " + item);
        }
    } else if (*it == "{") {
        throw std::logic_error("Invalid Context: " + item);
    }
    --it;
    return true;
}

int Parser::getSetting(string settingsList[], int size) {
    vector<string>           settings(settingsList, settingsList + size);
    vector<string>::iterator settingIt =
        find(settings.begin(), settings.end(), *it);
    if (settingIt == settings.end()) {
        return (-1);
    }
    return (settingIt - settings.begin());
}

bool Parser::setErrorLog() {
    validateFirstToken("error_log");
    httpConfig.error_log = *it;
    validateLastToken("error_log");
    return true;
}

bool Parser::setPid() {
    validateFirstToken("error_log");
    // httpConfig.pid = *it;
    validateLastToken("error_log");
    return true;
}

bool Parser::setGlobalSetting() {
    string List[] = {"error_log", "pid"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case ERROR_LOG:
            return setErrorLog();
        case PID:
            return setPid();
        default:
            throw std::invalid_argument("Invalid setting in global context: " +
                                        *it);
    }
}

bool Parser::setEventsContext() {
    if (context.back() != 0) {
        throw std::logic_error("Events context needs to be global.");
    }
    context.push_back(EVENTS);
    ++it;
    return true;
}

bool Parser::setEventsSetting() {
    string List[] = {"worker_connections"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case WORKER_CONNECTIONS:
            return setWorkerConnections();
        default:
            throw std::invalid_argument("Invalid setting: " + *it);
    }
}

bool Parser::setWorkerConnections() {
    validateFirstToken("worker_connections");
    int size = (*it).length();
    for (int i = 0; i < size; i++) {
        if (!isdigit((*it)[i]) || size > 10) {
            throw std::invalid_argument("Invalid worker_connections: " + *it);
        }
    }
    int num = stoi(*it);
    if (num > OPEN_MAX) {
        throw std::invalid_argument("worker_connections value over limit: " +
                                    *it);
    }
    // httpConfig.worker_connections = num;
    validateLastToken("worker_connections");
    return true;
}

bool Parser::setHttpContext() {
    if (context.back() != 0) {
        throw std::logic_error("Http context needs to be global.");
    } else if (httpConfig.servers.size() > 0) {
        throw std::logic_error("Only one http context is allowed.");
    }
    context.push_back(HTTP);
    ++it;
    return true;
}

bool Parser::setHttpSetting() {
    std::cout << "Http: ";
    string List[] = {"index"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case INDEX:
            std::cout << "Index: " << *(++it) << std::endl;
            break;
        default:
            throw std::invalid_argument("Invalid setting in Http context: " +
                                        *it);
    }
    ++it;
    return true;
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

int Parser::retrievePort(string num) {
    for (size_t i = 0; i < num.length(); ++i) {
        if (!isdigit(num[i])) {
            throw logic_error("Error: invalid port number for listen: " + num);
        }
    }
    return (atoi(num.c_str()));
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

string Parser::setLocationUri() {
    string path = *(++it);
    if (*(++it) != "{") {
        throw std::logic_error("Invalid syntax for location: " + *it);
    }
    (httpConfig.servers.back()).locations[path] = LocationConfig();
    ++it;
    return path;
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
