
#include "config.hpp"
#include "socket.hpp"
#include "webserv.hpp"

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
    if (argc == 1) {
        parseConfig(CONFIG_FILE);
    } else if (argc == 2) {
        parseConfig(argv[1]);
    } else {
        std::cerr << "Usage: ./webserv [config_file]" << std::endl;
        return (EXIT_FAILURE);
    }

    /** Create a socket for each server block */
    /** @todo create a for-loop to generate each socket */
    try {
        // Socket socket = Socket(httpConfig.servers[0].port, INADDR_ANY);
        Socket socket = Socket(3000, INADDR_ANY);
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }

    return (EXIT_SUCCESS);
}
