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

void parseConfig(std::string config_file, HttpConfig &httpConfig) {
    Parser        parser(httpConfig);
    std::string   line;
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

Parser::Parser(HttpConfig &httpConfig) : context(1), httpConfig(httpConfig) {}

Parser::~Parser(void) {
    tokens.clear();
}

void Parser::tokenizeConfig(std::string line) {
    while (line.size() > 0) {
        size_t pos = line.find_first_not_of(" \t");
        if (pos == line.npos || line[pos] == '#') {
            break;
        }
        line = line.substr(pos);
        pos  = line.find_first_of("\\#{}; \t\n\0");
        pos == 0 ? pos = 1 : pos;
        std::string tmp = line.substr(0, pos);
        tokens.push_back(tmp);
        ;
        line = line.substr(tmp.size());
    }
}

void Parser::validateFirstToken(std::string setting) {
    if (*(++it) == ";") {
        throw std::invalid_argument("Error: missing argument for " + setting);
    }
}

void Parser::validateLastToken(string setting) {
    if (*(++it) != ";") {
        throw std::invalid_argument("Error: too many arguments for " + setting + " (" + *it + ")");
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
    std::string item   = *it;
    std::string List[] = {"", "events", "http", "server"};
    int         tmp    = getSetting(List, sizeof(List) / sizeof(List[0]));
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

int Parser::getSetting(std::string settingsList[], int size) {
    std::vector<string>           settings(settingsList, settingsList + size);
    std::vector<string>::iterator settingIt = find(settings.begin(), settings.end(), *it);
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

bool Parser::setWorkerProcesses() {
    validateFirstToken("worker_processes");
    std::string num = *it;
    for (size_t i = 0; i < num.length(); i++) {
        if (!isdigit(num[i]) || num.length() > 10) {
            throw std::invalid_argument("Invalid worker_processes: " + num);
        }
    }
    if (stoi(num) != 1) {
        throw std::invalid_argument("Invalid worker_processes: this webserv is not multi threaded");
    }
    validateLastToken("worker_processes");
    return true;
}

bool Parser::setGlobalSetting() {
    std::string List[] = {"error_log", "pid", "worker_processes"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case ERROR_LOG:
            return setErrorLog();
        case PID:
            return setPid();
        case WORKER_PROCESSES:
            return setWorkerProcesses();
        default:
            throw std::invalid_argument("Invalid setting in global context: " + *it);
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
    std::string List[] = {"worker_connections"};
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
        throw std::invalid_argument("worker_connections value over limit: " + *it);
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

bool Parser::setIndex() {
    validateFirstToken("index");
    // httpConfig.index = *it;
    validateLastToken("index");
    return true;
}

bool Parser::setHttpSetting() {
    std::string List[] = {"index"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case INDEX:
            return setIndex();
        default:
            throw std::invalid_argument("Invalid setting in Http context: " + *it);
    }
}

bool Parser::setServerContext() {
    if (context.back() != 2) {
        throw std::logic_error("Server context needs to be inside http context.");
    }
    httpConfig.servers.push_back(ServerConfig());
    context.push_back(SERVER);
    ++it;
    return true;
}

bool Parser::setServerSetting() {
    std::string List[] = {"listen", "server_name", "access_log", "root", "location"};
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
            return setLocationUri();
        default:
            throw std::invalid_argument("Invalid setting in server context: " + *it);
    }
}

bool Parser::setListen() {
    validateFirstToken("server_name");
    std::string num = *it;
    if (num.find(":") == num.npos) {
        throw std::logic_error("Error: No address provided for listen");
    } else {
        std::string address = num.substr(0, num.find(":"));
        if (!isValidIPAddress(address)) {
            throw std::logic_error("Error: invalid IP address for listen: " + address);
        }
        num                                = num.substr(num.find(":") + 1);
        (httpConfig.servers.back()).listen = std::make_pair(address, retrievePort(num));
    }
    validateLastToken("listen");
    return (true);
}

bool Parser::isValidIPAddress(const std::string &ip) {
    std::regex pattern(
        "^([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
        "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
        "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
        "([01]?\\d\\d?|2[0-4]\\d|25[0-5])$");
    return std::regex_match(ip, pattern);
}

int Parser::retrievePort(std::string num) {
    for (size_t i = 0; i < num.length(); ++i) {
        if (!isdigit(num[i])) {
            throw std::logic_error("Error: invalid port number for listen: " + num);
        }
    }
    return (atoi(num.c_str()));
}

bool Parser::setServerName() {
    validateFirstToken("server_name");
    while (*it != ";") {
        (httpConfig.servers.back()).server_names.push_back(*it);
        ++it;
    }
    return (true);
}

bool Parser::setAccessLog() {
    validateFirstToken("access_log");
    while (*it != ";") {
        // std::cout << *it << " ";
        ++it;
    }
    return true;
}

bool Parser::setRoot() {
    validateFirstToken("root");
    //
    validateLastToken("root");
    return (true);
}

bool Parser::setLocationSetting(std::string uri) {
    std::string List[] = {"path:", "fastcgi:"};
    // string uri    = setLocationUri();
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
    // if (*(++it) != "}") {
    //     throw std::logic_error("Invalid syntax for location after setting: " +
    //                            *it);
    // }
    return true;
}

bool Parser::setLocationUri() {
    validateFirstToken("location");
    std::string uri = *it;
    if (*(++it) != "{") {
        throw std::logic_error("Invalid syntax for location: " + *it);
    }
    (httpConfig.servers.back()).locations[uri] = LocationConfig();
    while (*++it != "}") {
        setLocationSetting(uri);
    }

    // validateLastToken("location");
    return true;
}

void Parser::setPath(std::string &uri) {
    validateFirstToken("path");
    (httpConfig.servers.back()).locations[uri].root = *it;
    validateLastToken("path");
}

void Parser::setFastCGI(std::string &uri) {
    validateFirstToken("fastcgi");
    (httpConfig.servers.back()).locations[uri].cgi_path    = *it;
    (httpConfig.servers.back()).locations[uri].cgi_enabled = true;
    validateLastToken("fastcgi");
}
