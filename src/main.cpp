
#include "parsing.hpp"
#include "config.hpp"
#include "logging.hpp"
#include "server.hpp"

#ifndef CONFIG_FILE
#define CONFIG_FILE "config/webserv.conf"
#endif

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: ./webserv [config_file]" << std::endl;
        return (EXIT_FAILURE);
    }

    // Parse config file
    HttpConfig httpConfig;
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
        HttpServer httpServer(httpConfig);

        // Start the server
        httpServer.start();
    

    return (EXIT_SUCCESS);
}
