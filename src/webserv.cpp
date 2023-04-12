#include "webserv.hpp"

//**************************************************************************//
//                              Constructors                                //
//**************************************************************************//

Webserv::Webserv(void) : valid(false), configFile() {
}

Webserv::Webserv(const std::string &conf) : valid(false), configFile() {
	/* Print parsing error in the parsing */
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
}

//**************************************************************************//
//                        Member functions Tools                            //
//**************************************************************************//

//TODO Make it recursive to recall the function if the line content is http{server{listen 80...*/
bool Webserv::parseConfigLine(std::string line) {
	size_t pos = line.find_first_not_of(" \t\n");
	if (pos == line.npos){
		return (true);}
	std::string trim = line.substr(pos);
	if (trim.empty() || *trim.begin() == '#' || *trim.begin() == '{') {
		return (true);}
	size_t npos = trim.find_first_of(" #{");
	if (npos == trim.npos) {
	// One charactere management
		npos = 1;
		std::cout << trim << std::endl;
		return true;}
	std::string setting = trim.substr(0, npos);
	std::cout << setting << std::endl;
	// Recurse until end of line
	if (trim[npos] == '#') {
		return (true);}
	trim = trim.substr(npos + 1);
	parseConfigLine(trim);
	return (true);
}

std::string &Webserv::retrieveFilePath(std::string &conf) {
	char buf[PATH_MAX];

	realpath(conf.c_str(), buf);
	conf = std::string(buf);
	return (conf);
}