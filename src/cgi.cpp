#include "../include/cgi.hpp"

Cgi::Cgi(HttpRequest &request, LocationConfig &location, ServerConfig &config)
	: request_(request),
	  location_(location),
	  config_(config) { }

Cgi::~Cgi() { }

bool Cgi::exec() {
	try
	{
		this->extractScript();
		this->checkForScript();
		this->setEnv();
		this->handlePipe();
		return this->performCgi();
	}
	catch(const Cgi::RessourceDoesNotExist& e) {
		Logger::instance().log(e.what());
		this->handleError(Nonexistant);
		return false;
	}
	catch(const Cgi::InternalServerError& e) {
		Logger::instance().log(e.what());
		this->handleError(Internal);
		return false;
	}
	catch(const Cgi::ForbiddenFile& e) {
		Logger::instance().log(e.what());
		this->handleError(Permission);
		return false;
	}
	catch(const Cgi::InvalidPath& e) {
		Logger::instance().log(e.what());
		this->handleError(Access);
		return false;
	}
	catch(const std::exception& e) {
		Logger::instance().log(e.what());
		this->handleError(Internal);
		return false;
	}
}

bool Cgi::performCgi() {
	switch(request_.method_) {
		case GET:
			return performCgiGet();
			break;
		case POST:
			return performCgiPost();
			break;
		default:
			throw UnsupportedMethod();
	}
}

void Cgi::extractScript() { //prolly need to add more robust checking
	std::string uri = request_.uri_;
	std::string ext;

	for (size_t i = 0; i < location_.cgi_ext.size(); ++i) {
		if (uri.find(location_.cgi_ext[i]) != std::string::npos) {
			ext = location_.cgi_ext[i];
		}
	}
	if (!ext.size()) {
		throw RessourceDoesNotExist();
	}
	size_t pos = uri.find(ext);
	scriptWithPath_ = uri.substr(0, uri.find('/', pos));
	script_ = scriptWithPath_.substr(scriptWithPath_.rfind('/') + 1);
}

void Cgi::checkForScript() { //checks for file existence based on the request url and the root directive
	std::string actualPath(location_.root);
	actualPath.append(scriptWithPath_);
	if (access(actualPath.c_str(), F_OK)) {
		throw RessourceDoesNotExist();
	}
	if (access(actualPath.c_str(), X_OK)) {
		throw ForbiddenFile();
	}
}

static std::string myNtoaCauseFuckYou(struct in_addr in) { //You heard me right
	std::string buff;

	unsigned char *bytes = (unsigned char *) &in;

	buff.append(std::to_string((int) bytes[0]));
	buff.append(".");
	buff.append(std::to_string((int) bytes[1]));
	buff.append(".");
	buff.append(std::to_string((int) bytes[2]));
	buff.append(".");
	buff.append(std::to_string((int) bytes[3]));

	return buff;
}

void Cgi::setEnv() { // A lot of stuff happens here. The beginning of great things or something
	const struct sockaddr* addr = request_.currentSession->getSockaddr();
	const struct sockaddr_in *addrIn = (sockaddr_in *) addr;

	std::string uri = request_.uri_;
	std::string var;
	if (request_.body_.size() > 0) {
		var.append("CONTENT_LENGTH=");
		var.append(std::to_string(request_.body_.size()));
		meta_variables_.push_back(var);
		var.clear();
	}
	std::map<std::string, std::string>::iterator it = request_.headers_.find("Content-Type");
	if (it != request_.headers_.end()) {
		var.append("CONTENT_TYPE=");
		var.append(it->second);
		meta_variables_.push_back(var);
		var.clear();
	}
	meta_variables_.push_back("GATEWAY_INTERFACE=CGI/1.1");
	var.append("PATH_INFO=");
	var.append(uri.substr(0, uri.find('?')));
	meta_variables_.push_back(var);
	var.clear();
	if (uri.find('?') == std::string::npos) {
		meta_variables_.push_back("QUERY_STRING=");
	}
	else {
		var.append("QUERY_STRING=");
		var.append(uri.substr(uri.find('?') + 1));
		meta_variables_.push_back(var);
		var.clear();
	}
	var.append("REMOTE_ADDR=");
	var.append(myNtoaCauseFuckYou(addrIn->sin_addr));
	meta_variables_.push_back(var);
	var.clear();
	var.append("REQUEST_METHOD=");
	switch(request_.method_) {
		case 0:
			var.append("UNKNOWN");
			break;
		case 1:
			var.append("GET");
			break;
		case 2:
			var.append("POST");
			break;
		case 3:
			var.append("DELETE");
			break;
		default:
			throw InternalServerError();
	}
	meta_variables_.push_back(var);
	var.clear();
	var.append("SCRIPT_NAME=");
	var.append(scriptWithPath_);
	meta_variables_.push_back(var);
	var.clear();
	var.append("SERVER_PORT=");
	var.append(std::to_string(config_.listen.second));
	meta_variables_.push_back(var);
	var.clear();
	meta_variables_.push_back("SERVER_PROTOCOL=HTTP/1.1");

	for (size_t i = 0; i < meta_variables_.size(); ++i) {
		envp_[i] = &meta_variables_[i][0];
	}
	envp_[meta_variables_.size()] = NULL;
}

bool Cgi::performCgiGet() {
	return true;
}

bool Cgi::performCgiPost() {
	return true;
}

void Cgi::handlePipe() {

}

void Cgi::handleError(exceptionType type) {
	switch(type) {
		case (Internal):

			break;
		case (Permission):

			break;
		case (Access):

			break;
		case (Nonexistant):

			break;
		default:
			throw std::runtime_error("Something messed up happened");
			break;
	}

}

const char *Cgi::InternalServerError::what() const throw() {
	return "Internal server error exception";
}

const char *Cgi::ForbiddenFile::what() const throw() {
	return "Access forbidden";
}

const char *Cgi::InvalidPath::what() const throw() {
	return "Ressource does not exist";
}

const char *Cgi::UnsupportedMethod::what() const throw() {
	return "Unsupported http method";
}

const char *Cgi::RessourceDoesNotExist::what() const throw() {
	return "Requested ressource does not exist";
}