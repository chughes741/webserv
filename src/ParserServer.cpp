#include "Parser.hpp"

/**
 * @brief validate the value for port setting in listen
 * 
 * @param num the port value in string format
 * @return int if port value is all integer
 */
int	Parser::retrievePort(string num) {
	for (size_t i = 0; i < num.length(); ++i) {
		if (!isdigit(num[i])) {
			throw std::logic_error("Error: invalid port number for listen: " + num);
		}
	}
	return (std::stoi(num));
}

/**
 * @brief Transform the token into the listening port of the server
 * @return true or throw an exception
 */
bool Parser::setListen() {
	string num = *(++it);
	if (num.find(":") == num.npos) {
		int port = retrievePort(num);
		std:: cout << port << std::endl;
		(httpConfig.servers.back()).listen = std::make_pair("", port);
	}
	else {
		string address = num.substr(0, num.find(":"));
		num = num.substr(num.find(":") + 1);
		int port = retrievePort(num);
		(httpConfig.servers.back()).listen = std::make_pair(address, port);
	}
	++it;
	return (true);
}

/**
 * @brief Set the server name
 * 
 * @return true or throw invalid_argument
 */
bool Parser::setServerName() {
	std::cout << "Server_name: ";
	if (*(++it) == ";") {
		throw std::invalid_argument("Error: missing argument for server_name.");
	}
	(httpConfig.servers.back()).server_name = *it;
	// Erase if we only take one server_name and leave one ++it;
	while (*it != ";") {
		std::cout << *it << " ";
		++it;}
	std::cout <<std::endl;
	return (true);
}

bool Parser::setAccessLog() {
	std::cout << "Access_log: ";
	while (*(++it) != ";") {
		std::cout << *it << " ";}
	std::cout <<std::endl;
	return true;
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

bool Parser::setServerContext() {
	if (context.back() != 2) {
		throw std::logic_error("Server context needs to be inside http context.");}
	httpConfig.servers.push_back(ServerConfig());
	context.push_back(SERVER);
	++it;
	return true;
}