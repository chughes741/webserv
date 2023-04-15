#include "events.hpp"

//**************************************************************************//
//                              Constructors                                //
//**************************************************************************//

Events::Events(void) {
	settings.push_back("worker_connections");
	settings.push_back("use");
	settings.push_back("multi_accept");
	settings.push_back("accept_mutex_delay");
	settings.push_back("debug_connection");
	settings.push_back("use_poll");
	settings.push_back("deferred_accept");}

Events::Events(const Events &copy) {
	*this = copy;}

//**************************************************************************//
//                                 Setters                                  //
//**************************************************************************//

//**************************************************************************//
//                                 Getters                                  //
//**************************************************************************//

//**************************************************************************//
//                             Member functions                             //
//**************************************************************************//

bool Events::isSetting(std::string setting) {
	size_t pos = 0;
	for (; pos < settings.size(); ++pos) {
		if (setting.compare(settings.at(pos)) == 0) {
			return true;}}
	return false;
}

// Determine if the string following the setting is a valid one, which is an integer that ends with ";" 
bool Events::setWorkerConnections(std::string &value) {
	try {
		worker_connections = std::stoi(value);
		size_t i = 0;
		while (i < value.length() && std::isspace(value[i])) {
			++i;}
		while (i < value.length() && std::isdigit(value[i])) {
			++i;}
		while (i < value.length() && std::isspace(value[i])) {
			++i;}
		value = value.substr(i, value.length());
		if (value[i] != ';' || worker_connections < 1) {
			throw std::exception();}
	}
	catch (std::exception &e) {
		std::cerr << "Error: Invalid setting (events->worker_connections)" << std::endl;
		return (false);}
	return (true);
}

bool Events::setSetting(std::string setting, std::string &value) {
	size_t pos = 0;
	for (; pos < settings.size(); ++pos) {
		if (setting.compare(settings.at(pos)) == 0) {
			break;}}
	switch (pos) {
		case WORKER_CONNECTIONS:
			return (setWorkerConnections(value));
		case USE:
			return false;
		case MULTI_ACCEPT:
			return false;
		case ACCEPT_MUTEX_DELAY:
			return false;
		case DEBUG_CONNECTION:
			return false;
		case USE_POLL:
			return false;
		case DEFERRED_ACCEPT:
			return false;
		default:
			return false; 
	}
}

//**************************************************************************//
//                           Operators overload                             //
//**************************************************************************//

Events &Events::operator=(const Events &copy){
	if (this != &copy){}
	return (*this);}

//**************************************************************************//
//                               Destructors                                //
//**************************************************************************//

Events::~Events(void){}