
#ifndef WEBSERV_HPP
 #define WEBSERV_HPP

# include <iostream>
# include <cstdlib>
# include <unistd.h>
# include <fstream>
# include <vector>

# include "events.hpp"

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
//Functions tools
	std::string &retrieveFilePath(std::string &);
	bool parseConfigLine(std::string);
	bool isBlock(std::string setting);
	int isBlock(std::string setting);
protected:

private:
	bool valid;
	std::vector<std::string> configFile;
	std::vector<std::string> block;
	std::vector<Events> events;
};

#endif