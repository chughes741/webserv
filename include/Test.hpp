#pragma once

# include "config.hpp"
#include "sys/socket.h"
#include "fcntl.h"

#include <fstream>
#include <sstream>
#include <cstring>

using std::memset;

bool setBlocking(int fd, bool blocking) {
	if (fd < 0) {return false;}
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {return false;}
	flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
	return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
}

class Test {
public:
	Test() {
		int server_fd;
		struct sockaddr_in address;	
		int addrlen = sizeof(address);
		const int port = (httpConfig.servers.back()).listen.second;
		//const int port = 8080;
		
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
			perror("in socker");
			exit(EXIT_FAILURE);
		}
		int opt = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(port);

		memset(address.sin_zero, '\0', sizeof(address.sin_zero));

		if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
			perror("Bind failed");
			exit(EXIT_FAILURE);
		}
		if (listen(server_fd, 10) < 0) {
			perror("In listen");
			exit(EXIT_FAILURE);
		}
		int new_socket;
		while (1) {
			if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
				perror("In accept");
				exit(EXIT_FAILURE);
			}
			char buffer[30000] = {0};
			long valread = read(new_socket, buffer, 30000);
			if (valread < 0) {}
			std::cout << buffer << std::endl;
			// open and read the file
			std::string request(buffer);
			std::string resource;
			std::istringstream iss(request);
			std::string line;
			std::getline(iss, line);
			if (line.find("GET") != std::string::npos) {
				size_t start = line.find_first_of(" ") + 1;
				size_t end = line.find_last_of(" ") - 1;
				resource = line.substr(start, end - start + 1);
			}
			std::cout << resource << std::endl;
			std::ifstream file;
			if (resource.compare("/") == 0) {
				file.open("docs/_site/index.html");
			}
			else  {
				file.open("docs/_site" + resource);
			}
			std::stringstream file_buffer;
			file_buffer << file.rdbuf();
			file.close();
			std::string file_contents = file_buffer.str();
			setBlocking(new_socket, true);
			std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(file_contents.size()) + "\r\n\r\n" + file_contents + "\r\n";
			//write(new_socket, response.c_str(), response.size());
			send(new_socket, response.c_str(), response.size(), 0);
			//setBlocking(new_socket, false);
			close(new_socket);
			file_buffer.clear();
		}
		close(server_fd);
	}

private:

};