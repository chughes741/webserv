
#ifndef WEBSERV_HPP
 #define WEBSERV_HPP

# include <iostream>
# include <cstdlib>
# include <unistd.h>
# include <fstream>
# include <vector>

# include "events.hpp"
# include "Exception.hpp"
# include "config.hpp"

# define OUT	0
# define EVENTS 1
# define HTTP 	2
# define SERVER 3

class Webserv {
public:
//Default constructor (Required)
	Webserv();
//Copy constructor (Required)
	Webserv (const Webserv &);
//Constructors
	Webserv(const std::string &);
//Default Destructor (Required)
	~Webserv();
//Destructors
//Copy assignment operator (Required)
	Webserv & operator = (const Webserv &);
//Operators
//Swap function
//Setters & Getters
	bool getValid() {return (valid);}
//Other functions
	bool parseConfiguration(std::string);
	void printConfig() const;
	void printSettings() const;
//Functions tools
	std::string retrieveFilePath(const std::string &);
	bool parseConfigLine(std::string);
	int checkBlock(std::string setting);

/* trying new parsing */
	int isBlock(std::string &setting, bool create);
	void parseConfig(const std::string &);
	void readConfig(const std::string &);
	void parseItem(std::vector<std::string>::iterator &, int &, int &);
protected:

private:
	bool valid;
	std::vector<std::string> configFile;
	std::vector<std::string> configItems;
	std::vector<std::string> block;
	std::vector<Events *> events;
	HttpConfig httpConfig;
};

#endif