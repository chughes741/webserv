/**
 * @file ParserHttp.cpp
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

#define INDEX 0

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