/**
 * @file Parser.cpp
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

using std::find;

Parser::Parser(HttpConfig &httpConfig) : context(1), httpConfig(httpConfig) {
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

int Parser::getSetting(string settingsList[], int size) {
    vector<string>           settings(settingsList, settingsList + size);
    vector<string>::iterator settingIt =
        find(settings.begin(), settings.end(), *it);
    if (settingIt == settings.end()) {
        return (-1);
    }
    return (settingIt - settings.begin());
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

Parser::~Parser(void) {
    tokens.clear();
}

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
