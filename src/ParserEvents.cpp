#include "Parser.hpp"

bool Parser::setWorkerConnections() {
	std::cout << "worker_connections: " << *(++it) << std::endl;
	if (*(++it) != ";") {
		throw std::invalid_argument("Invalid value after Worker_Connections: " + *it);}
	return true;
}

bool Parser::setEventsSetting() {
	std::cout << "Events: ";
	string List[] = {"worker_connections"};
	switch (getSetting(List, sizeof(List)/sizeof(List[0]))) {
		case WORKER_CONNECTIONS:
			return setWorkerConnections();
		default:
			throw std::invalid_argument("Invalid setting: " + *it);
	}
}

bool Parser::setEventsContext() {
	if (context.back() != 0) {
		throw std::logic_error("Events context needs to be global.");}
	context.push_back(EVENTS);
	++it;
	return true;
}