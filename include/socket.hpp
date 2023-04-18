#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "events.hpp"
#include "webserv.hpp"

using namespace std;

#define SO_MAX_QUEUE 10
#define READ_BUFFER_SIZE 1024
#define CRLF "\r\n"

struct socket_addr_t {
    sockaddr  addr;
    socklen_t addr_len;
};

/**
 * @brief Socket class
 *
 * @todo accept() - accept a connection
 */
class Socket {
   public:
    /**
     * @brief Construct a new Socket object
     *
     * @param port Port to bind to
     * @param addr Address to bind to
     * @throw std::runtime_error
     */
    Socket(in_port_t port, in_addr_t addr) throw(runtime_error);

    /**
     * @brief Copy constructor
     */
    Socket(const Socket& other) throw();

    /**
     * @brief copy assignment operator
     */
    Socket& operator=(const Socket& other) throw();

    /**
     * @brief Destroy the Socket object
     */
    ~Socket() throw();

    /**
     * @brief Close the socket
     *
     * @throw std::runtime_error
     */
    void close() throw(runtime_error);

    /**
     * @brief Accept a connection and connects to it
     *
     * @throw std::runtime_error
     */
    void accept() throw(runtime_error);

    /**
     * @brief Calls send() on the socket
     *
     * @param buffer message to send
     * @throw std::runtime_error
     */
    void send(string buffer) const throw(runtime_error);

    /**
     * @brief Calls recv() on the socket
     *
     * @return string - message received
     * @throw std::runtime_error
     */
    string recv() const throw(runtime_error);

   public:
    int                     sockfd_;         /**< Socket file descriptor */
    struct sockaddr_in      addr_in_;        /**< Socket address */
    map<int, socket_addr_t> client_sockets_; /**< Map of client sockets */
};
