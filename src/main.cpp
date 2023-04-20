
#include "config.hpp"
#include "socket.hpp"
#include "webserv.hpp"
#include "server.hpp"

/** Maximum pending connections in queue */
#define SO_MAX_QUEUE 10

/** Global config object */
HttpConfig httpConfig = HttpConfig();

/**
 * @brief Main function
 *
 * @param argc Number of arguments
 * @param argv config file name
 */
int main(int argc, char* argv[]) {
    /** Parse the config file x*/
    try {
        if (argc == 1) {
            parseConfig(CONFIG_FILE);
        } else if (argc == 2) {
            parseConfig(argv[1]);
        } else {
            std::cerr << "Usage: ./webserv [config_file]" << std::endl;
            return (EXIT_FAILURE);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    ServerConfig config = ServerConfig();

    Server *server = new HttpServer(config);

    server->start();

    delete server;
    return (EXIT_SUCCESS);
}
