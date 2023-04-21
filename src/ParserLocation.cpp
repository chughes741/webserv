#include "Parser.hpp"

void Parser::setFastCGI() {
	std::cout << "Fastcgi: " << *(++it) << std::endl;
	if (*(++it) != ";") {
		throw std::logic_error("Invalid syntax for location after fastcgi: " + *it);}
}

void  Parser::setPath() {
	std::cout << "Path: " << *(++it) << std::endl;
	if (*(++it) != ";") {
		throw std::logic_error("Invalid syntax for location after path: " + *it);}
}

void Parser::setLocationUri() {
	string completePath = *(++it);
	if (*(++it) != "{") {
		throw std::logic_error("Invalid syntax for location: " + *it);}
	++it;
}

/**
 * @brief 		Set http setting
 */
bool Parser::setLocationSetting() {
	string List[] = {"path:", "fastcgi:"};
	setLocationUri();
	switch (getSetting(List, sizeof(List)/sizeof(List[0]))) {
		case PATH:
			setPath();
			break;
		case FASTCGI:
			setFastCGI();
			break;
		default:
			throw std::logic_error("Invalid setting for location: " + *it);
	}
	if (*(++it) != "}") {
		throw std::logic_error("Invalid syntax for location after setting: " + *it);}	
	return true;
}
