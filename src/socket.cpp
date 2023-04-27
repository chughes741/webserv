/**
 * @file socket.cpp
 * @author Francis L.
 * @author Marc-André L.
 * @author Cole H.
 * @version 0.1
 * @date 2023-04-19
 * @brief Class implementations for using sockets as defined in RFC 793
 */

#include "socket.hpp"

using std::atoi;
using std::make_pair;
using std::runtime_error;

void Session::send(int port, string buffer) const {
    ssize_t bytes_sent =
        ::send(port, buffer.c_str(), buffer.length(), MSG_DONTWAIT);
    if (bytes_sent == -1) {
        throw runtime_error("Error: Failed to send to socket");
    }
}

string Session::recv(int port) const {
    string buffer_str;
    char   buffer[READ_BUFFER_SIZE];
    int    bytes_received = ::recv(port, buffer, READ_BUFFER_SIZE, 0);

    while (bytes_received > 0) {
        buffer_str.append(buffer, bytes_received);
        bytes_received = ::recv(port, buffer, READ_BUFFER_SIZE, 0);
    }

    if (bytes_received == -1) {
        throw runtime_error("Error: Failed to receive from socket");
    }

    return buffer_str;
}

/** Used to make it a pure abstract class */
Socket::~Socket() {
}

TcpSocket::TcpSocket() {
    /**
     * @todo flags (isntead of 0) - these should be in setsockopt()
     *  SOCK_NONBLOCK? There's no define for it, O_NONBLOCK is a flag for open()
     *  SO_DEBUG might be useful
     *  SO_REUSEADDR might be useful
     *  SO_KEEPALIVE might be useful
     */
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1) {
        throw runtime_error("Error: Failed to create socket");
    }
}

TcpSocket::~TcpSocket() {
}

int TcpSocket::bind(string addr, int port) {
    addr_in_.sin_family      = AF_INET;                    // IPv4
    addr_in_.sin_port        = htons(port);                // Port
    addr_in_.sin_addr.s_addr = htonl(atoi(addr.c_str()));  // Addres

    // Binds socket to an address and port
    if (::bind(sockfd_, (struct sockaddr*)&addr_in_, sizeof(addr_in_)) == -1) {
        throw runtime_error("Error: Failed to bind socket");
    }

    return sockfd_;
}

void TcpSocket::listen() {
    // Sets server to listen passively
    if (::listen(sockfd_, SO_MAX_QUEUE) == -1) {
        throw runtime_error("Error: Failed to listen on socket");
    }
}

pair<int, Session> TcpSocket::accept() {
    struct sockaddr* client_addr     = new struct sockaddr;
    socklen_t        client_addr_len = sizeof(sockaddr);

    int client_sockfd = ::accept(sockfd_, client_addr, &client_addr_len);
    if (client_sockfd == -1) {
        delete client_addr;
        throw runtime_error("Error: Failed to accept connection");
    }

    Session session(client_sockfd, client_addr, client_addr_len);
    return make_pair(client_sockfd, session);
}

void TcpSocket::close() {
    if (::close(sockfd_) == -1) {
        throw runtime_error("Error: Failed to close socket");
    }
}
