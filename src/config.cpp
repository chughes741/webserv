#include "config.hpp"

/**
 * @brief Tokenize a line of a config file.
 * 
 * @param tokens	[in,out] Vector of tokens
 * @param line		[in] Line to tokenize
 */
void tokenizeConfig(vector<string> &tokens, string line) {
	while (line.size() > 0) {
		size_t pos = line.find_first_not_of(" \t");
		if (pos == line.npos || line[pos] == '#') {break;}
		line = line.substr(pos);
		pos = line.find_first_of("\\#{}; \t\n\0");
		pos == 0 ? pos = 1: pos;
		string tmp = line.substr(0, pos);
		tokens.push_back(tmp);;
		line = line.substr(tmp.size());
	}
}

/**
 * @brief Get the Setting object
 * 
 * @param settingsList		Array of settings
 * @param setting 			Setting to find
 * @param size 				Size of settings array
 * @return int				Index of setting in settings array
 */
int getSetting(string settingsList[], string &setting, int size) {
	vector<string> settings(settingsList, settingsList + size);
	vector<string>::iterator settingIt = find(settings.begin(), settings.end(), setting);
	if (settingIt == settings.end()) {
		return (-1);}
	return (settingIt - settings.begin());
}

#define WORKER_PROCESSES 0
#define ERROR_LOG 1
#define PID 2
/**
 * @brief 		Set global context settings
 * @param it	[in,out] Iterator of tokens
 */
void setGlobalSetting(vector<string>::iterator &it) {
	std::cout << "Global: ";
	string List[] = {"worker_processes", "error_log", "pid"};
	switch (getSetting(List, *it, sizeof(List)/sizeof(List[0]))) {
		case WORKER_PROCESSES:
			std::cout << "worker_processes: " << *(++it) << std::endl;
			break;
		case ERROR_LOG:
			std::cout << "error_log: " << *(++it) << std::endl;
			break;
		case PID:
			std::cout << "pid: " << *(++it) << std::endl;
			break;
		default:
			throw ConfigError((*it));
	}
	++it;
}

# define WORKER_CONNECTIONS 0
/**
 * @brief 		Set global context settings
 * @param it	[in,out] Iterator of tokens
 */
void setEventsSetting(vector<string>::iterator &it) {
	std::cout << "Events: ";
	string List[] = {"worker_connections"};
	switch (getSetting(List, *it, sizeof(List)/sizeof(List[0]))) {
		case WORKER_CONNECTIONS:
			std::cout << "worker_connections: " << *(++it) << std::endl;
			break;
		default:
			throw ConfigError((*it));
	}
	++it;
}

#define INDEX 0
/**
 * @brief 		Set http setting
 * @param it	[in,out] Iterator of tokens
 */
void setHttpSetting(vector<string>::iterator &it) {
	std::cout << "Http: ";
	string List[] = {"index"};
	switch (getSetting(List, *it, sizeof(List)/sizeof(List[0]))) {
		case INDEX:
			std::cout << "Index: " << *(++it) << std::endl;
			break;
		default:
			throw ConfigError((*it));
	}
	++it;
}
void setServerSetting(vector<string>::iterator &it);

#define PATH 1
#define FASTCGI 2
/**
 * @brief 		Set http setting
 * @param it	[in,out] Iterator of tokens
 */
void setLocation(vector<string>::iterator &it, string path, bool in) {
	string List[] = {"location", "path:", "fastcgi:"};
	(void) in;
	string completePath = path + *(++it);
	++it;
	switch (getSetting(List, *(++it), sizeof(List)/sizeof(List[0]))) {
		case 0:
			std::cout << "Location: " << *(++it) << std::endl;
			break;
		case PATH:
			std::cout << "Path: " << *(++it) << std::endl;
			break;
		case FASTCGI:
			std::cout << "Fastcgi: " << *(++it) << std::endl;
			break;
		default:
			throw ConfigError((*it));
	}
	++it;
}

#define LISTEN 0
#define SERVER_NAME 1
#define ACCESS_LOG 2
#define ROOT 3
#define LOCATION 4
/**
 * @brief 		Set http setting
 * @param it	[in,out] Iterator of tokens
 */
void setServerSetting(vector<string>::iterator &it) {
	std::cout << "Server: ";
	string List[] = {"listen", "server_name", "access_log", "root", "location"};
	switch (getSetting(List, *it, sizeof(List)/sizeof(List[0]))) {
		case LISTEN:
			std::cout << "Listen: " << *(++it) << std::endl;
			break;
		case SERVER_NAME:
			std::cout << "Server_name: ";
			while (*(++it) != ";") {
				std::cout << *it << " ";}
			std::cout <<std::endl;
			return;
		case ACCESS_LOG:
			std::cout << "Access_log: ";
			while (*(++it) != ";") {
				std::cout << *it << " ";}
			std::cout <<std::endl;
			return;
		case ROOT:
			std::cout << "Root: " << *(++it) << std::endl;
			break;
		case LOCATION:
			setLocation(it, "", false);
			break;
		default:
			throw ConfigError((*it));
	}
	++it;
}

#define GLOBAL	0
#define EVENTS	1
#define HTTP	2
#define SERVER	3
/**
 * @brief define in which context the token is
 * 
 */
void setContext(vector<string>::iterator &it, vector<int> &context) {
	string item = *it;
	string List[] = {"", "events", "http", "server"};
	int tmp = getSetting(List, item, sizeof(List)/sizeof(List[0]));
	if (*(++it) == "{" && tmp != -1) {
		switch (tmp) {
			case GLOBAL:
				break;
			case EVENTS:
				if (context.back() != 0) {
					throw ConfigError(item);}
				break;
			case HTTP:
				if (context.back() != 0) {
					throw ConfigError(item);}
				break;
			case SERVER:
				if (context.back() != 2) {
					throw ConfigError(item);}
				break;
			default:
				throw ConfigError(item);
		}
		context.push_back(tmp);
		++it;
		return;}
	else if (*it == "{") {
		throw ConfigError(item);}
	--it;
}

/**
 * @brief  Initialize settings from token vector
 * @param tokens	[in] Vector of tokens
 */
bool initSettings(vector<string> &tokens) {
	static vector<int> context(1);
	for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
		while (*it == "}" && !context.empty()) {
			context.pop_back();
			++it;}
		if (it  == tokens.end()) {
			break;}
		setContext(it, context);	
		if (context.back() == -1) {
			context.pop_back();}
		switch (context.back()) {
			case GLOBAL:
				setGlobalSetting(it);
				break;
			case EVENTS:
			 	setEventsSetting(it);
				break;
			case HTTP:
			 	setHttpSetting(it);
				break;
			case SERVER:
			 	setServerSetting(it);
				break;
			default:
				throw ConfigError((*it));
		}
	}
	if (context.size() != 1) {
		throw std::exception();}
	return (true);
}

/**
 * @brief Parse a config file. Read the file line by line and split into tokens.
 *
 * @param config_file	[in] Path to the config file
 */
void parseConfig(std::string config_file) {
	std::string line;
	std::ifstream file(config_file);
	std::vector<std::string> tokens;

	if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
 		throw FileError(config_file);}
	std::string filePath = std::string(realpath(config_file.c_str(), NULL));
 	if (PRINT) {
		std::cout << "# configuration file " << filePath << ":" << std::endl;}
 	while(getline(file, line)) {
		if (PRINT) {
			std::cout << line.substr(0, line.find_first_of("\n")) << std::endl;}
		tokenizeConfig(tokens, line);
	}
	file.close();
	initSettings(tokens);
}


// Determine if the string following the setting is a valid one, which is an integer that ends with ";" 
// void Events::setWorkerConnections(std::vector<std::string>::iterator &it) {
	// std::string num = *it;
	// for (size_t i = 0; i < num.length(); ++i) {
		// if (!std::isdigit(num[i])) {
			// throw std::exception();}
	// }
	// worker_connections = std::stoi(num);
	// if (*(++it) != ";" || worker_connections < 1) {
		// throw std::exception();}
// }

// void Events::setUse(std::vector<std::string>::iterator &it) {
	// std::string options[5] = {"epoll", "kqueue", "devpoll", "poll", "select"};
	// std::string setting = *it;
	// for (size_t pos = 0; pos < 5; ++pos) {
		// if (setting.compare(options[pos]) == 0) {
			// if (*(++it) != ";") {
				// throw WebExcep::WrongSettingValue("use");}
			// use = setting;
			// return;}
	// }
	// throw std::exception();
// }

// void Events::setSetting(std::string &setting, std::vector<std::string>::iterator &it) {
	// size_t pos = 0;
	// for (; pos < settings.size(); ++pos) {
		// if (setting.compare(settings.at(pos)) == 0) {
			// break;}}
	// switch (pos) {
		// case WORKER_CONNECTIONS:
			// setWorkerConnections(it);
			// break;
		// case USE:
			// setUse(it);
			// break;
		// case MULTI_ACCEPT:
			// break;
		// case ACCEPT_MUTEX_DELAY:
			// break;
		// case DEBUG_CONNECTION:
			// break;
		// case USE_POLL:
			// break;
		// case DEFERRED_ACCEPT:
			// break;
		// default:
			// throw WebExcep::UnknownDirective(setting); // unknown directive
	// }
// }
