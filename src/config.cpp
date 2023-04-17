#include <iostream>

#include "webserv.hpp"

/**
 * @brief Parse a config file
 *
 * @param config_file Path to the config file
 *
 * @return true if the config file was parsed successfully
 * @return false if the config file was not parsed successfully
 */
bool parseConfig(std::string config_file) {
    std::ifstream file(config_file);
    std::string   line;

    if (!file.is_open()) {
        std::cerr << "Error: Failed to open config file " << config_file
                  << std::endl;
        return (false);
    }

    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    return (true);
}
