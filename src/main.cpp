
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

    vector<int>    ports;
    vector<TcpSocket> sockets(ports.size());

    /** @todo get ports from config */
    ports.push_back(3000);

    /** Create a socket for each port */
    for (vector<TcpSocket>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
        try {
            *it = TcpSocket();
            (*it).bind(ports[it - sockets.begin()], INADDR_ANY);
            (*it).listen();
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    while (true) {
        /** @todo accept connections */
        break;
    }

    /** Close sockets */
    for (vector<TcpSocket>::iterator it = sockets.begin(); it != sockets.end();
         ++it) {
        try {
            it->close();
        } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    return (EXIT_SUCCESS);
}
