#include "webserv.hpp"

//**************************************************************************//
//                              Constructors                                //
//**************************************************************************//

Webserv::Webserv(void) : valid(false),
	configFile(),
	events() {
		block.push_back(std::string("out"));
		block.push_back(std::string("events"));
		block.push_back(std::string("http"));
		block.push_back(std::string("server"));
		block.push_back(std::string("location"));}

Webserv::Webserv(const std::string &conf) : valid(false), configFile(),
	events() {
	/* Print parsing error in the parsing */
	block.push_back(std::string("out"));
	block.push_back(std::string("events"));
	block.push_back(std::string("http"));
	block.push_back(std::string("server"));
	block.push_back(std::string("location"));
	valid = parseConfiguration(conf);
}

Webserv::Webserv(const Webserv &copy) {*this = copy;}

//**************************************************************************//
//                                 Setters                                  //
//**************************************************************************//

//**************************************************************************//
//                                 Getters                                  //
//**************************************************************************//

//**************************************************************************//
//                             Member functions                             //
//**************************************************************************//

bool Webserv::parseConfiguration(std::string conf) {
	std::string line;
	std::ifstream infile(conf);

	//TODO Verify if file ./ should return error
	if (!infile.is_open()) {
		return (false);}
	std::string filePath = retrieveFilePath(conf);
	configFile.push_back("# configuration file " + filePath + ":");
	while(getline(infile, line)) {
		if (parseConfigLine(line) == false) {
			infile.close();
			return (false);}
		configFile.push_back(line);}
	infile.close();
	return (true);
}

void	Webserv::printConfig() const {
	std::vector<std::string>::const_iterator it = configFile.begin();
	std::vector<std::string>::const_iterator it_end = configFile.end();
	for (; it != it_end; ++it) {
		std::cout << *it << std::endl;}
}

void	Webserv::printSettings() const {
	std::cout << "--EVENTS--" << std::endl;
	std::cout << "Worker_Connections:" << events[0]->getWorkerConnections() << std::endl;
}

//**************************************************************************//
//                           Operators overload                             //
//**************************************************************************//

Webserv &Webserv::operator=(const Webserv &copy){
	if (this != &copy){
		configFile.clear();
		configFile = copy.configFile;
		valid = copy.valid;
	}
	return (*this);}

//**************************************************************************//
//                               Destructors                                //
//**************************************************************************//

Webserv::~Webserv(void){
	std::vector<Events *>::const_iterator it_end = events.end();
	for (std::vector<Events *>::const_iterator it = events.begin(); it != it_end; ++it) {
		delete *it;
	}
}

//**************************************************************************//
//                        Member functions Tools                            //
//**************************************************************************//

bool isComment(std::string &line) {
	try {
		if (line.at(line.find_first_not_of(" \t")) == '#') {
			return true;}
	}
	catch (std::exception &e) {
		return true;}
	line = line.substr(line.find_first_not_of(" \t"));
	return false;
}

bool isSetting(std::string line) {
	std::string trim;
	try {
		trim = line.substr(0, line.find_first_of(" \t"));
	}
	catch (std::exception &e) {
		return false;}
	return true;
}

bool Webserv::isBlock(std::string setting) {
	for (size_t i = 1; i < block.size();i++) {
		if (setting == block.at(i)) {
			return (true);}
	}
	return (false);
} 

int Webserv::checkBlock(std::string setting) {
	size_t pos = 1;
	for (; pos < block.size(); ++pos) {
		if (setting == block.at(pos)) {
			break;}}
	switch (pos) {
		case EVENTS:
			if (events.size() > 0){
				throw (std::exception());}
			events.push_back(new Events());
			return (pos);
		default:
			throw (std::exception());
	}
	return (OUT);
}


//TODO Make it recursive to recall the function if the line content is http{server{listen 80...*/
bool Webserv::parseConfigLine(std::string line) {
	static int bracket = OUT;

	if (isComment(line) || !isSetting(line)) {
		return true;}
	std::string setting = line.substr(0, line.find_first_of("{ \t"));
	if (isBlock(setting)) {
		try {
			bracket = checkBlock(setting);}
		catch (std::exception &e) {
			return (false);}
		return (true);}
	switch (bracket) {
		case EVENTS:
			if (events.front()->isSetting(setting)) {
				line = line.substr(setting.length());
				return (events.front()->setSetting(setting, line));}
			break;
		case HTTP:
			return true;
		case SERVER:
			return true;
		case LOCATION:
			return true;
		default:;
	}
	return true;
}
//Verify every element is well close after

std::string &Webserv::retrieveFilePath(std::string &conf) {
	char buf[PATH_MAX];

	realpath(conf.c_str(), buf);
	conf = std::string(buf);
	return (conf);
}