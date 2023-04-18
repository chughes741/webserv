
// Webserv::Webserv(const std::string &conf) : valid(false), configFile() {
// 	block.push_back(std::string("out"));
// 	block.push_back(std::string("events"));
// 	block.push_back(std::string("http"));
// 	block.push_back(std::string("server"));
// 	parseConfig(conf);
// }

// /* Determine if we are entering a new block settings */
// int Webserv::isBlock(std::string &setting, bool create) {
// 	for (size_t i = 1; i < block.size();i++) {
// 		if (setting == block.at(i)) {
// 			if (!create) {
// 				return (i);}
// 			switch (i) {
// 				case EVENTS:
// 					if (!events.empty()) {
// 						// TODO define exception, can only be 1 events block
// 						throw std::exception();
// 					}
// 					events.push_back(new Events());
// 					break;
// 				default:;
// 			}
// 			return (i);}
// 	}
// 	throw WebExcep::WrongContext(setting);
// } 

// void Webserv::parseItem(std::vector<std::string>::iterator &it, int &block, int &level) {
// 	std::string item = *it;
// 	std::string next = *(++it);
// 	if (it != configItems.end() && next.compare("{") == 0 && isBlock(item, false)) {
// 		block = isBlock(item, true);
// 		++level;}
// 	else {
// 		switch (block) {
// 		case OUT:
// 			break;
// 		case EVENTS:
// 			events[events.size() - 1]->setSetting(item, it);
// 			break;
// 		case HTTP:
// 			break;
// 		case SERVER:
// 			break;
// 		default:
// 			// TODO Find why we could end up here and define the exception
// 			throw std::exception();}
// 	}
// 	if ((*(++it)).compare("}") == 0) {
// 		--block;
// 		--level;
// 		if (level == 0) {
// 			block = 0;}
// 	}
// 	else {
// 		--it;}
// }

// void Webserv::parseConfig(const std::string &conf) {
// 	static int block = OUT;
// 	static int level = 0;
// 	readConfig(conf);
// 	std::vector<std::string>::iterator it = configItems.begin();
// 	std::vector<std::string>::iterator it_end = configItems.end();
// 	for (; it != it_end; ++it) {
// 		parseItem(it, block, level);
// 	}
// 	if (level != OUT)
// 	// Means all brackets are not properly closed
// 		throw std::exception();
// }

// //**************************************************************************//
// //                             Print tools                                  //
// //**************************************************************************//

// void	Webserv::printConfig() const {
// 	std::vector<std::string>::const_iterator it = configFile.begin();
// 	std::vector<std::string>::const_iterator it_end = configFile.end();
// 	for (; it != it_end; ++it) {
// 		std::cout << *it << std::endl;}
// }

// void	Webserv::printSettings() const {
// 	if (events.size() > 0)  {
// 		std::cout << "--EVENTS--" << std::endl;
// 		std::cout << "Worker_Connections:" << events[0]->getWorkerConnections() << std::endl;
// 		std::cout << "Use:" << events[0]->getUse() << std::endl;
// 	}
// }

// //**************************************************************************//
// //                           Operators overload                             //
// //**************************************************************************//

// Webserv &Webserv::operator=(const Webserv &copy){
// 	if (this != &copy){
// 		configFile.clear();
// 		configFile = copy.configFile;
// 		valid = copy.valid;
// 	}
// 	return (*this);}

// //**************************************************************************//
// //                               Destructors                                //
// //**************************************************************************//

// Webserv::~Webserv(void){
// 	std::vector<Events *>::const_iterator it_end = events.end();
// 	for (std::vector<Events *>::const_iterator it = events.begin(); it != it_end; ++it) {
// 		delete *it;
// 	}
// }

// //**************************************************************************//
// //                        Member functions Tools                            //
// //**************************************************************************//

// std::string Webserv::retrieveFilePath(const std::string &conf) {
// 	char buf[PATH_MAX];

// 	realpath(conf.c_str(), buf);
// 	return(std::string(buf));
// }