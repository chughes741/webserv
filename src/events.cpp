#include "events.hpp"

//**************************************************************************//
//                              Constructors                                //
//**************************************************************************//

Events::Events(void) : settings() {
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
	std::vector<std::string>::const_iterator it_end = settings.end();
	for (std::vector<std::string>::const_iterator it = settings.begin(); it != it_end; ++it) {
		if (setting.compare(*it)) {
			return true;
		}
	}
	return false;
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