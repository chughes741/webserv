#ifndef CONFIGBLOCK_HPP
 #define CONFIGBLOCK_HPP

# include <iostream>
# include <vector>

//Template 
class ConfigBlock {
public:
//Default constructor (Required)
	ConfigBlock();
//Copy constructor (Required)
	ConfigBlock (const ConfigBlock &copy);
//Constructors
//Default Destructor (Required)
	~ConfigBlock();
//Destructors
//Copy assignment operator (Required)
	ConfigBlock & operator = (const ConfigBlock &copy);
//Operators
//Swap function
//Setters & Getters
//Other functions
	bool validateType(); //from enum launch pointer function to validate type
protected:

private:
	//Enum blockType? Events;http;server;location
	std::string blockType;
	std::vector<ConfigBlock> asda

	//All configurations available
	//server_name ...;

};

#endif