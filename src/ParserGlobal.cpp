/**
 * @file ParserGlobal.cpp
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

bool setWorkerProcessses(string num) {
    if (num == "auto") {
    }
    return true;
}

bool Parser::setGlobalSetting() {
    std::cout << "Global: ";
    string List[] = {"worker_processes", "error_log", "pid"};
    switch (getSetting(List, sizeof(List) / sizeof(List[0]))) {
        case WORKER_PROCESSES:
            // setWorkerProcessses();
            std::cout << "worker_processes: " << *(++it) << std::endl;
            break;
        case ERROR_LOG:
            httpConfig.error_log = *(++it);
            std::cout << "error_log: " << *(it) << std::endl;
            break;
        case PID:
            // httpConfig.pid_file = *(++it);
            ++it;
            std::cout << "pid: " << *(it) << std::endl;
            break;
        default:
            throw std::invalid_argument("Invalid setting in global context: " +
                                        *it);
    }
    ++it;
    return true;
}
