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

/**
 * @brief Represents a TCP client connection
 */
class Session {
   public:
    Session(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
        : sockfd_(sockfd), addr_(addr), addrlen_(addrlen) {}

    int                    sockfd() const { return sockfd_; }
    const struct sockaddr* addr() const { return addr_; }
    socklen_t              addrlen() const { return addrlen_; }

   private:
    int                    sockfd_;  /**< Session socket file descriptor */
    const struct sockaddr* addr_;    /**< Session socket address */
    socklen_t              addrlen_; /**< Session socket address length */
};

/**
 * @brief Socket class
 */
class Socket {
   public:
    Socket() {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ == -1) {
            throw std::runtime_error("Error: Failed to create socket");
        }
    }
    virtual ~Socket() throw() {}

   private:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wreturn-type"
    Socket(const Socket& other) {}
    Socket& operator=(const Socket& other) {}
#pragma clang diagnostic pop

   public:
    virtual void   bind(in_port_t port, in_addr_t addr)   = 0;
    virtual void   listen()                               = 0;
    virtual void   accept()                               = 0;
    virtual void   close()                                = 0;
    virtual void   send(int client, string message) const = 0;
    virtual string recv(int client) const                 = 0;

   protected:
    int                sockfd_;   /**< Server socket file descriptor */
    struct sockaddr_in addr_in_;  /**< Server address */
    map<int, Session>  sessions_; /**< Map of client sockets */
};

/**
 * @brief Represents a TCP socket
 */
class TcpSocket : public Socket {
   public:
    /**
     * @brief Construct a new TcpSocket object
     */
    TcpSocket();

    /**
     * @brief Destroy the TcpSocket object
     */
    ~TcpSocket() throw();

    /**
     * @brief Bind the socket to a port and address
     *
     * @param port
     * @param addr
     */
    void bind(in_port_t port, in_addr_t addr);

    /**
     * @brief Listen for connections
     */
    void listen();

    /**
     * @brief Accept a connection and connects to it
     *
     * @throw std::runtime_error
     */
    void accept();

    /**
     * @brief Close the socket
     *
     * @throw std::runtime_error
     */
    void close();

    /**
     * @brief Calls send() on the socket
     *
     * @param port port to send to
     * @param buffer message to send
     * @throw std::runtime_error
     */
    void send(int client, string message) const;

    /**
     * @brief Calls recv() on the socket
     *
     * @param port port to receive from
     * @return string - message received
     * @throw std::runtime_error
     */
    string recv(int client) const;
};
