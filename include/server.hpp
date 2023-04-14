#ifndef SERVER_HPP
 #define SERVER_HPP

# include <iostream>

class Server {
public:
//Default constructor (Required)
	Server();
//Copy constructor (Required)
	Server (const Server &copy);
//Constructors
//Default Destructor (Required)
	~Server();
//Destructors
//Copy assignment operator (Required)
	Server & operator = (const Server &copy);
//Operators
//Swap function
//Setters & Getters
//Other functions
protected:

private:
	bool bracketClose;
};

#endif