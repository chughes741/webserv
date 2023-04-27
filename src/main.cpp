
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

    // Initialize server
    HttpServer httpServer = HttpServer(httpConfig);

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
