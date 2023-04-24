
#include "config.hpp"
#include "socket.hpp"
#include "Parser.hpp"
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
    // Exit if the number of arguments is greater than 2
	if (argc > 2) {
		std::cerr << "Usage: ./webserv [config_file]" << std::endl;
		return (EXIT_FAILURE);
	}

    // Parse config file
	httpConfig = HttpConfig();
	try {
		if (argc == 2) {
			parseConfig(argv[1]);
		} else {
			parseConfig(CONFIG_FILE);
		}
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

    // Create server
	ServerConfig test_config;
    Server *server = new HttpServer(test_config);

    server->start();

    delete server;
    return (EXIT_SUCCESS);
}
