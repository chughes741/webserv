#include "webserv.hpp"

/* If no arguments, default configurations? */
int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Error: Incorrect number of arguments." << std::endl;
		std::cerr << "Usage: ./webserv \"filename\".conf" << std::endl;
		return (EXIT_FAILURE);}
	else {
		/* Create class from conf file*/
		Webserv webserv( (std::string(argv[1])) );
		if (!webserv.getValid()) {
			// std::cout << "Bad configuration" << std::endl;
			return (EXIT_FAILURE);
		}
		else {
			//webserv.printConfig();
		}
	}
}