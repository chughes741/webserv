
#ifndef WEBSERV_HPP
 #define WEBSERV_HPP

# include <iostream>
# include <fstream>

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
	bool parseConfiguration(const std::string &);
protected:

private:
	bool valid;
};

#endif