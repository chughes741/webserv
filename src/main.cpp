
#include "config.hpp"
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
int main(int argc, char *argv[]) {
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
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

    /** Create listeners for each server block */
    for (std::vector<ServerConfig>::iterator it = httpConfig.servers.begin();
         it != httpConfig.servers.end(); it++) {
        if (listen(it->port, SO_MAX_QUEUE) == -1) {
            std::cerr << "Error: Failed to listen on port " << it->port
                      << std::endl;
            return (EXIT_FAILURE);
        } else {
            std::cout << "Listening on port " << it->port << std::endl;
        }
    }

    return (EXIT_SUCCESS);
}
