#include "socket.hpp"

using namespace std;

Socket::Socket(in_port_t port, in_addr_t addr) throw(runtime_error) {
    client_sockets_ = map<int, socket_addr_t>();

    /** Creates a socket */
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1) {
        throw std::runtime_error("Error: Failed to create socket");
    }

    /** Set socket options */
    // if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) <
        // 0) {
        // throw std::runtime_error("Error: Failed to set socket options");
    // }

    addr_in_.sin_family      = AF_INET;     /**< IPv4 */
    addr_in_.sin_port        = htons(port); /**< Port */
    addr_in_.sin_addr.s_addr = htonl(addr); /**< Address */

    /** Binds socket to an address and port */
    if (bind(sockfd_, (struct sockaddr*)&addr_in_, sizeof(addr_in_)) == -1) {
        throw std::runtime_error("Error: Failed to bind socket");
    }

    /** Sets server to listen passively */
    if (listen(sockfd_, SO_MAX_QUEUE) == -1) {
        throw std::runtime_error("Error: Failed to listen on socket");
    }
}

Socket::Socket(const Socket& other) throw() {
    sockfd_  = other.sockfd_;
    addr_in_ = other.addr_in_;
}

Socket& Socket::operator=(const Socket& other) throw() {
    if (this == &other) {
        return *this;
    }

    sockfd_  = other.sockfd_;
    addr_in_ = other.addr_in_;

    return *this;
}

Socket::~Socket() throw(runtime_error) {
    if (close(sockfd_) == -1) {
        throw std::runtime_error("Error: Failed to close socket");
    }
}

void Socket::accept() throw(runtime_error) {
    socket_addr_t socket_addr;
    socket_addr.addr_len = sizeof(socket_addr.addr);

    int socket_fd = ::accept(sockfd_, &socket_addr.addr, &socket_addr.addr_len);
    if (socket_fd == -1) {
        throw std::runtime_error("Error: Failed to accept connection");
    }

    /** Blocking */
    if (connect(socket_fd, &socket_addr.addr, socket_addr.addr_len) == -1) {
        throw std::runtime_error("Error: Failed to connect to client");
    }

    client_sockets_.insert(make_pair(socket_fd, socket_addr));
}

void Socket::send(string buffer) const throw(runtime_error) {
    ssize_t bytes_sent =
        ::send(sockfd_, buffer.c_str(), buffer.length(), MSG_DONTWAIT);
    if (bytes_sent == -1) {
        throw std::runtime_error("Error: Failed to send to socket");
    }
}

string Socket::recv() const throw(runtime_error) {
    string buffer_str;
    char   buffer[READ_BUFFER_SIZE];
    int    bytes_received = ::recv(sockfd_, buffer, READ_BUFFER_SIZE, 0);

    while (bytes_received > 0) {
        buffer_str.append(buffer, bytes_received);
        bytes_received = ::recv(sockfd_, buffer, READ_BUFFER_SIZE, 0);
    }

    if (bytes_received == -1) {
        throw std::runtime_error("Error: Failed to receive from socket");
    }

    return buffer_str;
}

/**
 * @brief Read a request from the socket
 *
 * @return Request
 */
Request readRequest(Socket socket) {
    Request request;

    string buffer = socket.recv();

    /** start-line */
    request.method = buffer.substr(0, buffer.find(' '));
    buffer.erase(0, buffer.find(' ') + 1);
    request.uri = buffer.substr(0, buffer.find(' '));
    buffer.erase(0, buffer.find(' ') + 1);
    request.version = buffer.substr(0, buffer.find(CRLF));
    buffer.erase(0, buffer.find(CRLF) + 2);

    /** body */
    request.body = buffer.substr(buffer.find("\r\n\r\n") + 4);
    buffer.erase(buffer.find("\r\n\r\n") + 2);

    /** headers */
    while (buffer.find(CRLF) != string::npos) {
        string key = buffer.substr(0, buffer.find(':'));
        buffer.erase(0, buffer.find(':') + 2);
        string value = buffer.substr(0, buffer.find(CRLF));
        buffer.erase(0, buffer.find(CRLF) + 2);
        request.headers[key] = value;
    }

    return request;
}

/**
 * @brief Write a response to the socket
 *
 * @param response Response to write
 */
void writeResponse(Socket socket, Response response) {
    string buffer;

    /** status-line */
    buffer.append(response.version + " ");
    buffer.append(response.status + " ");
    buffer.append(response.server + CRLF);

    /** headers */
    for (map<string, string>::iterator it = response.headers.begin();
         it != response.headers.end(); ++it) {
        buffer.append(it->first + ": " + it->second + CRLF);
    }

    /** body */
    buffer.append(CRLF + response.body);

    socket.send(buffer);
}
