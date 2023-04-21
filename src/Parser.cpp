#include "Parser.hpp"

//**************************************************************************//
//                              Constructors                                //
//**************************************************************************//

Parser::Parser(void) : context(1) {}

//**************************************************************************//
//                                 Setters                                  //
//**************************************************************************//

//**************************************************************************//
//                                 Getters                                  //
//**************************************************************************//

//**************************************************************************//
//                             Member functions                             //
//**************************************************************************//

bool Parser::contextSwitchCase(int c) {
	switch (c) {
		case GLOBAL:
			return setGlobalSetting();
		case EVENTS:
			return setEventsSetting();
		case HTTP:
			return setHttpSetting();
		case SERVER:
			return setServerSetting();
		default:
			throw std::logic_error("Invalid context: " + *it);
	}
	return true;
}

/**
 * @brief Get the Setting object
 * 
 * @param settingsList		Array of settings
 * @param setting 			Setting to find
 * @param size 				Size of settings array
 * @return int				Index of setting in settings array
 */
int Parser::getSetting(string settingsList[], int size) {
	vector<string> settings(settingsList, settingsList + size);
	vector<string>::iterator settingIt = find(settings.begin(), settings.end(), *it);
	if (settingIt == settings.end()) {
		return (-1);}
	return (settingIt - settings.begin());
}

/**
 * @brief define in which context the token is
 * 
 */
void Parser::setContext() {
	string item = *it;
	string List[] = {"", "events", "http", "server"};
	int tmp = getSetting(List, sizeof(List)/sizeof(List[0]));
	if (*(++it) == "{" && tmp != -1) {
		switch (tmp) {
			case GLOBAL:
				break;
			case EVENTS:
				if (context.back() != 0) {
					throw std::logic_error("Wrong context item: " + item);}
				break;
			case HTTP:
				if (context.back() != 0) {
					throw std::logic_error("Wrong context item: " + item);}
				break;
			case SERVER:
				if (context.back() != 2) {
					throw std::logic_error("Wrong context item: " + item);}
				httpConfig.servers.push_back(ServerConfig());
				break;
			default:
				throw std::logic_error("Invalid context: " + item);
		}
		context.push_back(tmp);
		++it;
		return;}
	else if (*it == "{") {
		throw std::logic_error("Invalid Context: " + item);}
	--it;
}

/**
 * @brief  Initialize settings from token vector
 * @param tokens	[in] Vector of tokens
 */
void Parser::initSettings() {
	it = tokens.begin();
	for (; it != tokens.end(); ++it) {
		while (*it == "}" && !context.empty()) {
			context.pop_back();
			++it;}
		if (it  == tokens.end()) {
			break;}
		setContext();	
		if (context.back() == -1) {
			context.pop_back();}
		contextSwitchCase(context.back());
	}
	if (context.size() != 1) {
		throw std::exception();}
}

/**
 * @brief Tokenize a line of a config file.
 * @param line		[in] Line to tokenize
 */
void Parser::tokenizeConfig(string line) {
	while (line.size() > 0) {
		size_t pos = line.find_first_not_of(" \t");
		if (pos == line.npos || line[pos] == '#') {
			break;}
		line = line.substr(pos);
		pos = line.find_first_of("\\#{}; \t\n\0");
		pos == 0 ? pos = 1: pos;
		string tmp = line.substr(0, pos);
		tokens.push_back(tmp);;
		line = line.substr(tmp.size());
	}
}

//**************************************************************************//
//                           Operators overload                             //
//**************************************************************************//

//**************************************************************************//
//                               Destructors                                //
//**************************************************************************//

Parser::~Parser(void){}

//**************************************************************************//
//                             Outside Functions                            //
//**************************************************************************//

/**
 * @brief Parse a config file. Read the file line by line and split into tokens.
 * @param config_file	[in] Path to the config file
 */
void parseConfig(string config_file) {
	Parser parser;
	string line;
	std::ifstream file(config_file);

	if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
		throw std::invalid_argument("File not found:  " + config_file);}
 	while(getline(file, line)) {
		parser.tokenizeConfig(line);
	}
	file.close();
	parser.initSettings();
}