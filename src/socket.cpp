/**
 * @file socket.cpp
 * @brief Class implementations for using sockets as defined in RFC 793
 *
 * This file contains the class implementations for using sockets as defined in
 * RFC 793.
 *
 * @note This code is for educational purposes only and should not be used in
 * production environments without extensive testing and modification.
 *
 * @version 0.1
 * @date 2021-04-19
 * @authors
 *  - Francis L.
 *  - Marc-André L.
 *  - Cole H.
 */

#include "socket.hpp"

Session::Session(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
    : sockfd_(sockfd), addr_(addr), addrlen_(addrlen) {}

Session::~Session() {}

int Session::getSockFd() const {
    return sockfd_;
}

TcpSession::TcpSession(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
    : Session(sockfd, addr, addrlen) {}

void TcpSession::send(int port, std::string buffer) const {
    ssize_t bytes_sent = ::send(port, buffer.c_str(), buffer.length(), MSG_DONTWAIT);
    if (bytes_sent == -1) {
        throw std::runtime_error("Error: Failed to send to socket");
    }
}

std::string TcpSession::recv(int client) const {
    std::string buffer_str;
    char        buffer[READ_BUFFER_SIZE + 1];
    ssize_t     bytes_received;

    do {
        bytes_received = ::recv(client, buffer, READ_BUFFER_SIZE, 0);
        buffer[bytes_received] = '\0';
        buffer_str.append(buffer, bytes_received);
    } while (bytes_received == READ_BUFFER_SIZE);

    if (bytes_received == -1) {
        Logger::instance().log("Error: Failed to receive from socket");
        // throw std::runtime_error("Error: Failed to receive from socket");
    }

    return buffer_str;
}

Session* tcp_session_generator(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    return new TcpSession(sockfd, addr, addrlen);
}

Socket::Socket(SessionGenerator session_generator) : session_generator_(session_generator) {}

/** Used to make it a pure abstract class */
Socket::~Socket() {}

TcpSocket::TcpSocket(SessionGenerator session_generator) : Socket(session_generator) {
    /**
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
    if (fcntl(sockfd_, F_SETFL, O_NONBLOCK) == -1) {
        throw std::runtime_error("Error: Failed to set socket to non-blocking");
    }
}

TcpSocket::~TcpSocket() {}

int TcpSocket::bind(std::string addr, int port) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;  // AF_INET indicates IPv4
    hints.ai_socktype = SOCK_STREAM;

    // Convert the port number to a string because getaddrinfo needs it that way
    char port_str[6];  // max port number is 65535, 5 characters + null terminator
    snprintf(port_str, sizeof(port_str), "%d", port);

    int status = getaddrinfo(addr.c_str(), port_str, &hints, &res);
    if (status != 0) {
        throw std::runtime_error(std::string("Error: getaddrinfo failed with error: ") +
                                 gai_strerror(status));
    }

    // Copy the address information to addr_in_
    memcpy(&addr_in_, res->ai_addr, sizeof(addr_in_));

    // Binds socket to an address and port
    if (::bind(sockfd_, (struct sockaddr*)&addr_in_, sizeof(addr_in_)) == -1) {
        freeaddrinfo(res);
        throw std::runtime_error(std::string("Error: Failed to bind socket: ") + strerror(errno));
    }

    freeaddrinfo(res);
    return sockfd_;
}

void TcpSocket::listen() {
    // Sets server to listen passively
    if (::listen(sockfd_, SO_MAX_QUEUE) == -1) {
        throw std::runtime_error(std::string("Error: Failed to listen on socket: ") +
                                 strerror(errno));
    }
}

Session* TcpSocket::accept() {
    struct sockaddr* client_addr     = new struct sockaddr;
    socklen_t        client_addr_len = sizeof(sockaddr);

    int client_sockfd = ::accept(sockfd_, client_addr, &client_addr_len);
    if (client_sockfd == -1) {
        delete client_addr;
        throw std::runtime_error("Error: Failed to accept connection");
    }

    return session_generator_(client_sockfd, client_addr, client_addr_len);
}

void TcpSocket::close() {
    if (::close(sockfd_) == -1) {
        throw std::runtime_error("Error: Failed to close socket");
    }
}

Socket* tcp_socket_generator() {
    return new TcpSocket();
}
