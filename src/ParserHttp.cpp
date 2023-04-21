#include "Parser.hpp"

#define INDEX 0
/**
 * @brief 		Set http setting
 * @param it	[in,out] Iterator of tokens
 */
bool Parser::setHttpSetting() {
	std::cout << "Http: ";
	string List[] = {"index"};
	switch (getSetting(List, sizeof(List)/sizeof(List[0]))) {
		case INDEX:
			std::cout << "Index: " << *(++it) << std::endl;
			break;
		default:
			throw std::invalid_argument("Invalid setting in Http context: " + *it);
	}
	++it;
	return true;
}