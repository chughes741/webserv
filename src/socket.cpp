#include "socket.hpp"

using namespace std;

/** C++ is a silly language */
Socket::~Socket() {}

TcpSocket::TcpSocket() {
    sessions_ = map<int, Session>();

    /** Creates a socket
     *
     * @todo flags (isntead of 0) - these should be in setsockopt()
     *  SOCK_NONBLOCK? There's no define for it, O_NONBLOCK is a flag for open()
     *  SO_DEBUG might be useful
     *  SO_REUSEADDR might be useful
     *  SO_KEEPALIVE might be useful
     */
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1) {
        throw std::runtime_error("Error: Failed to create socket");
    }
}

TcpSocket::~TcpSocket() throw() {}

void TcpSocket::bind(in_port_t port, in_addr_t addr) {
    addr_in_.sin_family      = AF_INET;     /**< IPv4 */
    addr_in_.sin_port        = htons(port); /**< Port */
    addr_in_.sin_addr.s_addr = htonl(addr); /**< Address */

    /** Binds socket to an address and port */
    if (::bind(sockfd_, (struct sockaddr*)&addr_in_, sizeof(addr_in_)) == -1) {
        throw std::runtime_error("Error: Failed to bind socket");
    }
}

void TcpSocket::listen() {
    /** Sets server to listen passively */
    if (::listen(sockfd_, SO_MAX_QUEUE) == -1) {
        throw std::runtime_error("Error: Failed to listen on socket");
    }
}

void TcpSocket::accept() {
    struct sockaddr* client_addr     = new struct sockaddr;
    socklen_t        client_addr_len = sizeof(sockaddr);

    int client_sockfd = ::accept(sockfd_, client_addr, &client_addr_len);
    if (client_sockfd == -1) {
        delete client_addr;
        throw std::runtime_error("Error: Failed to accept connection");
    }

    Session session(client_sockfd, client_addr, client_addr_len);
    sessions_.insert(make_pair(client_sockfd, session));
}

void TcpSocket::close() {
    if (::close(sockfd_) == -1) {
        throw std::runtime_error("Error: Failed to close socket");
    }
}

void TcpSocket::send(int port, string buffer) const {
    ssize_t bytes_sent =
        ::send(port, buffer.c_str(), buffer.length(), MSG_DONTWAIT);
    if (bytes_sent == -1) {
        throw std::runtime_error("Error: Failed to send to socket");
    }
}

string TcpSocket::recv(int port) const {
    string buffer_str;
    char   buffer[READ_BUFFER_SIZE];
    int    bytes_received = ::recv(port, buffer, READ_BUFFER_SIZE, 0);

    while (bytes_received > 0) {
        buffer_str.append(buffer, bytes_received);
        bytes_received = ::recv(port, buffer, READ_BUFFER_SIZE, 0);
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
 *
 * @todo put into an HttpServer class
 */
Request readRequest(TcpSocket socket, int port) {
    Request request;

    string buffer = socket.recv(port);

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
 *
 * @todo put into an HttpServer class
 */
void writeResponse(int port, TcpSocket socket, Response response) {
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

    socket.send(port, buffer);
}
