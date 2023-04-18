// #include "events.hpp"

// using std::cerr;
// using std::cout;
// using std::endl;
// using std::string;

// /** @todo check if this buffersize should be larger */
// #define READ_BUFFER_SIZE 1024
// #define CRLF "\r\n"

// /**
//  * @brief Read a request from a socket
//  *
//  * @param socket socket to read from
//  * @return Request the request that was read
//  */
// Request readRequest(int socket) {
//     Request request;

//     char   buffer[READ_BUFFER_SIZE];
//     string buffer_string;
//     int    bytes_read = recv(socket, buffer, READ_BUFFER_SIZE, 0);

//     while (bytes_read > 0) {
//         buffer_string.append(buffer, bytes_read);
//         bytes_read = recv(socket, buffer, READ_BUFFER_SIZE, 0);
//     }

//     if (bytes_read == -1) {
//         cerr << "Error reading from socket" << endl;
//     }

//     /** start-line */
//     request.method = buffer_string.substr(0, buffer_string.find(' '));
//     buffer_string.erase(0, buffer_string.find(' ') + 1);
//     request.uri = buffer_string.substr(0, buffer_string.find(' '));
//     buffer_string.erase(0, buffer_string.find(' ') + 1);
//     request.version = buffer_string.substr(0, buffer_string.find(CRLF));
//     buffer_string.erase(0, buffer_string.find(CRLF) + 2);

//     /** body */
//     request.body = buffer_string.substr(buffer_string.find("\r\n\r\n") + 4);
//     buffer_string.erase(buffer_string.find("\r\n\r\n") + 2);

//     /** headers */
// 	while (buffer_string.find(CRLF) != string::npos) {
// 		string key = buffer_string.substr(0, buffer_string.find(':'));
// 		buffer_string.erase(0, buffer_string.find(':') + 2);
// 		string value = buffer_string.substr(0, buffer_string.find(CRLF));
// 		buffer_string.erase(0, buffer_string.find(CRLF) + 2);
// 		request.headers[key] = value;
// 	}

//     return request;
// }

// void writeResponse(int socket, Response response) {
//     string buffer;

//     /** status-line */
//     buffer.append(response.version + " ");
//     buffer.append(response.status + " ");
//     buffer.append(response.server + CRLF);

//     /** headers */
// 	for (map<string, string>::iterator it = response.headers.begin();
// 		 it != response.headers.end(); ++it) {
// 		buffer.append(it->first + ": " + it->second + CRLF);
// 	}

//     /** body */
//     buffer.append(CRLF + response.body);

//     send(socket, buffer.c_str(), buffer.length(), MSG_DONTWAIT);
// }
