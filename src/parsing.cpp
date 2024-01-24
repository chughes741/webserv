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

void printHttpConfig(const HttpConfig& config) {
    std::cout << "HttpConfig values:" << std::endl;
    std::cout << "Http Context -> error_log: " << config.error_log << std::endl;
    std::cout << "Servers:" << std::endl;
    for (std::vector<ServerConfig>::const_iterator serverIt = config.servers.begin(); serverIt != config.servers.end(); ++serverIt) {
        const ServerConfig& server = *serverIt;

        std::cout << "Server Names: ";
        for (std::vector<std::string>::const_iterator nameIt = server.server_names.begin(); nameIt != server.server_names.end(); ++nameIt) {
            std::cout << *nameIt << " ";
        }
        std::cout << std::endl;
        std::cout << "Listen: " << server.listen.first << ":" << server.listen.second << std::endl;
        std::cout << "Root: " << server.root << std::endl;
        std::cout << "Error Pages:" << std::endl;
        for (std::map<int, std::string>::const_iterator errorIt = server.error_page.begin(); errorIt != server.error_page.end(); ++errorIt) {
            std::cout << "HTTP " << errorIt->first << ": " << errorIt->second << std::endl;
        }
        std::cout << "Client Max Body Size: " << server.client_max_body_size << std::endl;
        std::cout << "Locations:" << std::endl;
        for (std::map<std::string, LocationConfig>::const_iterator locationIt = server.locations.begin(); locationIt != server.locations.end(); ++locationIt) {
            const LocationConfig& location = locationIt->second;
            std::cout << "Path: " << locationIt->first << std::endl;
            std::cout << "  Client Max Body Size: " << location.client_max_body_size << std::endl;
            std::cout << "  Error Pages:" << std::endl;
            for (std::map<int, std::string>::const_iterator errorIt = location.error_page.begin(); errorIt != location.error_page.end(); ++errorIt) {
                std::cout << "  HTTP " << errorIt->first << ": " << errorIt->second << std::endl;
            }
            std::cout << "  Root: " << location.root << std::endl;
            std::cout << "  Index File: " << location.index_file << std::endl;
            std::cout << "  CGI Enabled: " << (location.cgi_enabled ? "true" : "false") << std::endl;
            for (size_t i = 0; i < location.cgi_ext.size(); ++i) {
                std::cout << "  Cgi_ext " << i << ": " << location.cgi_ext[i] << std::endl; 
            }
        }
    }
    std::cout << "Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator errorIt = config.error_page.begin(); errorIt != config.error_page.end(); ++errorIt) {
        std::cout << "HTTP " << errorIt->first << ": " << errorIt->second << std::endl;
    }
    std::cout << "Error Log: " << config.error_log << std::endl;
    std::cout << "Root: " << config.root << std::endl;
    std::cout << "Client Max Body Size: " << config.client_max_body_size << std::endl;
}


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
    // printHttpConfig(httpConfig);
}

Parser::Parser(HttpConfig &httpConfig) : tokens(0), context(1), httpConfig(httpConfig) {}

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
        line = line.substr(tmp.size());
    }
}

void Parser::validateFirstToken(std::string setting) {
    if (*(++it) == ";") {
        throw std::invalid_argument("Error: missing argument for " + setting);
    }
}

void Parser::validateLastToken(std::string setting) {
    if (*(++it) != ";") {
        throw std::invalid_argument("Error: too many arguments for " + setting + " (" + *it + ")");
    }
}

void Parser::initSettings() {
    it = tokens.begin();
    for (; it != tokens.end(); ++it) {
        while (it != tokens.end() && *it == "}" && !context.empty()) {
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
    std::vector<std::string>           settings(settingsList, settingsList + size);
    std::vector<std::string>::iterator settingIt = find(settings.begin(), settings.end(), *it);
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
        case 0:
            return setErrorLog();
        case 1:
            return setPid();
        case 2:
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
        case 0:
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
    validateLastToken("index");
    return true;
}

bool Parser::createOrValidateDirectory() {
    std::string dir_path;
    int fd = 0;
    if ((*it).front() == '/') {
        fd = open((*it).c_str(), O_DIRECTORY, O_CREAT);
    } else {
        char buf[2048];
        getwd(buf);
        *it = std::string(buf) + "/" + *it;
        fd = open((*it).c_str(), O_DIRECTORY, O_CREAT);
    }
    if (fd < 0)
        throw std::invalid_argument("Invalid upload directory: " + *it);
    else
        close(fd);
    return true;
}

bool Parser::setHttpUploadDirectory() {
    validateFirstToken("upload_dir");
    createOrValidateDirectory();
    httpConfig.upload_dir = *it;
    validateLastToken("upload_dir");
    return true;
}

bool Parser::setHttpSetting() {
    std::string List[] = {"index", "error_page", "client_max_body_size", "upload_dir"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case 0:
            return setIndex();
        case 1:
            return setHttpErrorPage();
        case 2:
            return setHttpClientBodySize();
        case 3:
            return setHttpUploadDirectory();
        default:
            throw std::invalid_argument("Invalid setting in Http context: " + *it);
    }
}

bool Parser::setHttpErrorPage() {
    validateFirstToken("error_page");
    int error = std::stoi(*it);
    std::string error_file = *++it;
    if (error_file == ";") {
        throw std::logic_error("Error: missing file path for error " + std::to_string(error));
    } else {
        httpConfig.error_page[error] = error_file;
    }
    validateLastToken("error_page");
    return true;
}

bool    Parser::setHttpClientBodySize() {
    validateFirstToken("client_max_body_size");
    std::string value = *it;
    size_t end = value.find_first_not_of("0123456789");
    if (end == value.npos) {
        httpConfig.client_max_body_size = std::stoi(value);
    } else if (end == value.size() - 1) {
        switch(value.at(end)){
            case 'm': case 'M':
                httpConfig.client_max_body_size = std::stoi(value) * 1024 * 1024;
                break;
            case 'k': case 'K':
                httpConfig.client_max_body_size = std::stoi(value) * 1024;
                break;
            default:
                throw std::logic_error("Invalid client_max_body_size in http context");
        }
    } else {
        throw std::logic_error("Invalid client_max_body_size in http context");
    }
    validateLastToken("client_max_body_size");
    httpConfig.max_body_size = true;
    return true;
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

bool Parser::setServerUploadDirectory() {
    validateFirstToken("upload_dir");
    createOrValidateDirectory();
    httpConfig.servers.back().upload_dir = *it;
    validateLastToken("upload_dir");
    return true;
}

bool Parser::setServerSetting() {
    std::string List[] = {"listen", "server_name", "error_page", "root", "location",
         "client_max_body_size", "return", "upload_dir"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case 0:
            return setListen();
        case 1:
            return setServerName();
        case 2:
            return setServerErrorPage();
        case 3:
            return setServerRoot();
        case 4:
            return setLocationUri();
        case 5:
            return setServerClientBodySize();
        case 6:
            return setServerRedirect();
        case 7:
            return setServerUploadDirectory();
        default:
            throw std::invalid_argument("Invalid setting in server context: " + *it);
    }
}

bool Parser::setServerRedirect() {
    validateFirstToken("return");
    std::string error_code = *it;
    try {
        int num = std::stoi(error_code);
        if (num == 300 || num == 301) {
            httpConfig.servers.back().redirect.first = num;
        } else {
            throw std::invalid_argument("Wrong error code");
        }
    } catch (...) {
        std::cout << "Invalid Error Code" << std::endl;
    }
    std::string redirect_url = *++it;
    if (redirect_url != ";") {
        httpConfig.servers.back().redirect.second = redirect_url;
    } else {
        throw std::invalid_argument("No redirect url");
    }
    validateLastToken("return");
    return true;
}

bool Parser::setListen() {
    validateFirstToken("listen");
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

bool Parser::setServerErrorPage() {
    validateFirstToken("error_page");
    int error = std::stoi(*it);
    std::string error_file = *++it;
    if (error_file == ";") {
        throw std::logic_error("Error: missing file path for error " + std::to_string(error));
    } else {
        (httpConfig.servers.back()).error_page[error] = error_file;
    }
    validateLastToken("error_page");
    return true;
}

bool Parser::setServerRoot() {
    validateFirstToken("root");
    std::string root = *it;
    (httpConfig.servers.back()).root = root;
    validateLastToken("root");
    return (true);
}

bool    Parser::setServerClientBodySize() {
    validateFirstToken("client_max_body_size");
    std::string value = *it;
    size_t end = value.find_first_not_of("0123456789");
    if (end == value.size()) {
        (httpConfig.servers.back()).client_max_body_size = std::stoi(value);
    } else if (end == value.size() - 1) {
        switch(value.at(end)){
            case 'm': case 'M':
                (httpConfig.servers.back()).client_max_body_size = std::stoi(value) * 1024 * 1024;
                break;
            case 'k': case 'K':
                (httpConfig.servers.back()).client_max_body_size = std::stoi(value) * 1024;
                break;
            default:
                throw std::logic_error("Invalid client_max_body_size in server context");
        }
    } else {
        throw std::logic_error("Invalid client_max_body_size in server context");
    }
    validateLastToken("client_max_body_size");
    (httpConfig.servers.back()).max_body_size = true;
    return true;
}

bool Parser::setLocationUploadDirectory(std::string &uri) {
    validateFirstToken("upload_dir");
    createOrValidateDirectory();
    httpConfig.servers.back().locations[uri].upload_dir = *it;
    validateLastToken("upload_dir");
    return true;
}

bool Parser::setLocationSetting(std::string uri) {
    std::string List[] = {"root", "cgi:", "autoindex", "error_page", "limit_except",
        "client_max_body_size","return", "upload_dir"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case 0:
            return setLocationRoot(uri);
        case 1:
            return setFastCGI(uri);
        case 2:
            return setAutoIndex(uri);
        case 3:
            return setLocationErrorPage(uri);
        case 4:
            return setLimitExcept(uri);
        case 5:
            return setLocationClientBodySize(uri);
        case 6:
            return setLocationRedirect(uri);
        case 7:
            return setLocationUploadDirectory(uri);
        default:
            throw std::logic_error("Invalid setting for location: " + *it);
    }
    return true;
}

bool Parser::setLocationRedirect(std::string &uri) {
    validateFirstToken("return");
    std::string error_code = *it;
    try {
        int num = std::stoi(error_code);
        if (num == 300 || num == 301) {
            httpConfig.servers.back().locations[uri].redirect.first = num;
        } else {
            throw std::invalid_argument("Wrong error code");
        }
    } catch (...) {
        std::cout << "Invalid Error Code" << std::endl;
    }
    std::string redirect_url = *++it;
    if (redirect_url != ";") {
        httpConfig.servers.back().locations[uri].redirect.second = redirect_url;
    } else {
        throw std::invalid_argument("No redirect url");
    }
    validateLastToken("return");
    return true;
}

bool Parser::setLocationUri() {
    validateFirstToken("location");
    std::string uri = *it;
    if (*(++it) != "{") {
        throw std::logic_error("Invalid syntax for location: " + *it);
    }
    (httpConfig.servers.back()).locations[uri] = LocationConfig();
    (httpConfig.servers.back()).locations[uri].limit_except.push_back(1);
    (httpConfig.servers.back()).locations[uri].limit_except.push_back(2);
    (httpConfig.servers.back()).locations[uri].limit_except.push_back(3);
    while (*++it != "}") {
        setLocationSetting(uri);
    }
    return true;
}

bool Parser::setLocationRoot(std::string &uri) {
    validateFirstToken("root");
    std::string root = *it;
    (httpConfig.servers.back()).locations[uri].root = root;
    validateLastToken("root");
    return true;
}

bool Parser::setFastCGI(std::string &uri) {
    validateFirstToken("cgi");
    (httpConfig.servers.back()).locations[uri].cgi_ext.push_back(*it);
    (httpConfig.servers.back()).locations[uri].cgi_enabled = true;
    validateLastToken("cgi");
    return true;
}

bool Parser::setAutoIndex(std::string &uri) {
    validateFirstToken("autoindex");
    if (*it != "on")
        throw std::logic_error("Error: wrong value for autoindex: " + *it);
    (httpConfig.servers.back()).locations[uri].autoindex = true;
    validateLastToken("autoindex");
    return true;
}

bool Parser::setLocationErrorPage(std::string &uri) {
    validateFirstToken("error_page");
    int error = std::stoi(*it);
    std::string error_file = *++it;
    if (error_file == ";") {
        throw std::logic_error("Error: missing file path for error " + std::to_string(error));
    } else {
        (httpConfig.servers.back()).locations[uri].error_page[error] = error_file;
    }
    validateLastToken("error_page");
    return true;
}

bool Parser::setLimitExcept(std::string &uri) {
    validateFirstToken("limit_except");
    (httpConfig.servers.back()).locations[uri].limit_except.clear();
    while (it != tokens.end() &&  *it != ";") {
        if (*it == "GET")
            (httpConfig.servers.back()).locations[uri].limit_except.push_back(1);
        else if (*it == "POST")
            (httpConfig.servers.back()).locations[uri].limit_except.push_back(2);
        else if (*it == "DELETE")
            (httpConfig.servers.back()).locations[uri].limit_except.push_back(3);
        else
            throw std::logic_error("Error: wrong method (" +*it + ") for location " + uri);
        *it++;
    }
    return true;
}

bool    Parser::setLocationClientBodySize(std::string &uri) {
    validateFirstToken("client_max_body_size");
    std::string value = *it;
    size_t end = value.find_first_not_of("0123456789");
    if (end == value.size()) {
        (httpConfig.servers.back()).locations[uri].client_max_body_size = std::stoi(value);
    } else if (end == value.size() - 1) {
        switch(value.at(end)){
            case 'm': case 'M':
                (httpConfig.servers.back()).locations[uri].client_max_body_size = std::stoi(value) * 1024 * 1024;
                break;
            case 'k': case 'K':
                (httpConfig.servers.back()).locations[uri].client_max_body_size = std::stoi(value) * 1024;
                break;
            default:
                throw std::logic_error("Invalid client_max_body_size in location context");
        }
    } else {
        throw std::logic_error("Invalid client_max_body_size in location context");
    }
    validateLastToken("client_max_body_size");
    (httpConfig.servers.back()).locations[uri].max_body_size = true;
    return true;
}
