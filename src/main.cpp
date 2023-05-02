/**
 * @file main.cpp
 * @brief Main function
 *
 * This file contains the main function for the web server. It parses the
 * configuration file, creates an HttpServer object, and then runs the server.
 *
 * @note This code is for educational purposes only and should not be used in
 * production environments without extensive testing and modification.
 *
 * @version 0.1
 * @date 2023-04-19
 * @authors
 *  - Francis L.
 *  - Marc-André L.
 *  - Cole H.
 */

#include <stdlib.h>

#include "webserv.hpp"

/**
 * @brief Main function
 *
 * @param argc Number of arguments
 * @param argv [1] config file name
 */
int main(int argc, char *argv[]) {
    // Exit if the number of arguments is greater than 2
    if (argc > 2) {
        std::cerr << "Usage: ./webserv [config_file]" << std::endl;
        return (EXIT_FAILURE);
    }

    HttpConfig httpConfig;
    // Parse config file
    try {
        if (argc == 2) {
            parseConfig(argv[1], httpConfig);
        } else {
            parseConfig(CONFIG_FILE, httpConfig);
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    // Create a listener
    EventListener *listener;
#ifdef __APPLE__
    listener = new KqueueEventListener();
#elif __linux__
    listener = new EpollEventListener();
#else
#error "Unsupported platform"
#endif  // __APPLE__

    // Initialize server
    HttpServer httpServer(httpConfig, listener);

    // Run server
    while (true) {
        try {
            httpServer.start();
        } catch (
            std::exception &e) { /** @todo needs a more specific exception */
            std::cerr << e.what() << std::endl;
            httpServer.stop();
        }
    }

    return (EXIT_SUCCESS);
}
