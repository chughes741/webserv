#include "Parser.hpp"

/**
 * @brief Transform the token into the listening port of the server
 * 
 * @return true or throw an exception
 */
bool Parser::setListen() {
	string num = *(++it);
	for (size_t i = 0; i < num.length(); ++i) {
		if (!isdigit(num[i])) {
			throw std::logic_error("Invalid port number for listen: " + num);
		}
	}
	// (httpConfig.servers.back()) = stoi(num);
	std::cout << "Listen: " << *(it) << std::endl;
	++it;
	return (true);
}

bool Parser::setServerName() {
	std::cout << "Server_name: ";
		while (*(++it) != ";") {
			std::cout << *it << " ";}
		std::cout <<std::endl;
	return (true);
}

bool Parser::setAccessLog() {
	std::cout << "Access_log: ";
	while (*(++it) != ";") {
		std::cout << *it << " ";}
	std::cout <<std::endl;
	return (true);
}

bool Parser::setRoot() {
	std::cout << "Root: " << *(++it) << std::endl;
	++it;
	return (true);
}

/**
 * @brief 		Set server setting
 */
bool Parser::setServerSetting() {
	std::cout << "Server: ";
	string List[] = {"listen", "server_name", "access_log", "root", "location"};
	switch (getSetting(List, sizeof(List)/sizeof(List[0]))) {
		case LISTEN:
			return setListen();
		case SERVER_NAME:
			return setServerName();
		case ACCESS_LOG:
			return setAccessLog();
		case ROOT:
			return setRoot();
		case LOCATION:
			return setLocationSetting();
		default:
			throw std::invalid_argument("Invalid setting in server context: " + *it);
	}
	return (true);
}