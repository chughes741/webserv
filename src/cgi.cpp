#include "../include/cgi.hpp"

Cgi::Cgi(HttpRequest &request, LocationConfig &location, ServerConfig &config, HttpResponse& response)
	: request_(request),
	  location_(location),
	  config_(config),
	  response_(&response) { }

Cgi::~Cgi() { }

bool Cgi::exec() {
	try
	{
		this->extractScript();
		this->checkForScript();
		this->setEnv();
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
	if (access(actualPath.c_str(), F_OK) == -1) {
		throw RessourceDoesNotExist();
	}
	if (access(actualPath.c_str(), X_OK) == -1) {
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
	envp_[meta_variables_.size()] = nullptr;
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

bool Cgi::performCgiGet() {
	std::string workingDirectory;
	char *argv[2];

	argv[0] = &script_[0];
	argv[1] = nullptr;

	workingDirectory = this->config_.root.append(scriptWithPath_.substr(0, scriptWithPath_.find_last_of('/')));

	int fd[2];
	int pid;
	if (pipe(fd) == -1) {
		Logger::instance().log("Pipe failed");
		throw InternalServerError();
	}
	pid = fork();
	if (pid == -1) {
		Logger::instance().log("Fork failed");
		throw InternalServerError();
	}
	if (pid == 0) {
		int result = chdir(workingDirectory.c_str());
		if (result == -1) {
			Logger::instance().log("From child: Failed to change working directory");
			exit(-1);
		}
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		execve(argv[0], argv, envp_);
		std::string error("From child: Execve failed: ");
		error.append(strerror(errno));
		Logger::instance().log(error);
		exit(-1);
	}
	else {
		int status = 0;
		std::string scriptOutput;
		close(fd[1]);
		char buffer[1024];
		while (read(fd[0], buffer, 1024) > 0) {
			scriptOutput.append(buffer);
			bzero(buffer, 1024);
		}
		extractHeaders(scriptOutput);
		extractBody(scriptOutput);
		waitpid(pid, &status, 0);
		if (WEXITSTATUS(status) != 0) {
			Logger::instance().log("Script execution failed");
			throw InternalServerError();
		}
		else {
			if (response_->headers_.find("Status") == response_->headers_.end()) {
				response_->headers_["Status"] = OK;
			}
		}
	}

	return true;
}

bool Cgi::performCgiPost() {
	return true;
}

void Cgi::extractHeaders(std::string scriptOutput) {
	std::string headerFields;
	std::size_t boundary = scriptOutput.find("\n\n");
	std::vector<std::pair<std::string, std::string> > headers;
	std::string field;
	std::size_t fieldBoundary;
	if (boundary == std::string::npos)
		return;
	else {
		headerFields = scriptOutput.substr(0, boundary + 1);
		while (boundary != std::string::npos) {
			boundary = headerFields.find('\n');
			if (boundary == std::string::npos)
				break;
			fieldBoundary = headerFields.find(':');
			if (fieldBoundary == std::string::npos)
				break;
			if (fieldBoundary == std::string::npos || (fieldBoundary > boundary)) {
				headerFields = headerFields.substr(boundary);
			}
			else {
				headers.push_back(std::make_pair(headerFields.substr(0, fieldBoundary), headerFields.substr(fieldBoundary, (boundary - fieldBoundary))));
				headerFields = headerFields.substr(boundary + 1);
			}
		}
		for (std::size_t i = 0; i < headers.size(); ++i) {
			response_->headers_[headers[i].first] = headers[i].second;
		}
	}
}

void Cgi::extractBody(std::string scriptOutput) {
	std::string body;
	std::size_t boundary = scriptOutput.find("\n\n");
	if (boundary == std::string::npos) {
		response_->body_ = scriptOutput;
		return;
	}
	else {
		std::string body = scriptOutput.substr(boundary);
		response_->body_ = body;
	}
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