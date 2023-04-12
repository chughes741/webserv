
#ifndef WEBSERV_HPP
 #define WEBSERV_HPP

# include <iostream>
# include <cstdlib>
# include <unistd.h>
# include <string>
# include <fstream>
# include <vector>

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
protected:

private:
	bool valid;
	std::vector<std::string> configFile;
};

#endif