/**
 * @file socket.hpp
 * @author Francis L.
 * @author Marc-André L.
 * @author Cole H.
 * @version 0.1
 * @date 2023-04-19
 * @brief Classes for using sockets as defined in RFC 793
 */

#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "webserv.hpp"

using std::map;
using std::pair;
using std::string;

#define SO_MAX_QUEUE 10
#define READ_BUFFER_SIZE 1024
#define CRLF "\r\n"

/**
 * @brief Pure abstract class representing a socket session
 */
class Session {
   public:
    Session(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    virtual ~Session() = 0;

    virtual void   send(int client, string message) const = 0;
    virtual string recv(int client) const                 = 0;

   protected:
    int                    sockfd_;  /**< Session socket file descriptor */
    const struct sockaddr* addr_;    /**< Session socket address */
    socklen_t              addrlen_; /**< Session socket address length */
};

/**
 * @brief Represents a TCP session
 */
class TcpSession : public Session {
   public:
    TcpSession(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

    void   send(int client, string message) const;
    string recv(int client) const;
};

/** TcpSession generator function */
Session* tcp_session_generator(int sockfd, const struct sockaddr* addr,
                               socklen_t addrlen);

/**
 * @brief Socket base class
 * @pure
 */
class Socket {
   public:
    typedef Session* (*SessionGenerator)(int                    sockfd,
                                         const struct sockaddr* addr,
                                         socklen_t              addrlen);

    Socket(SessionGenerator session_generator);
    virtual ~Socket() = 0;

    virtual int      bind(string addr, int port) = 0;
    virtual void     listen()                    = 0;
    virtual Session* accept()                    = 0;
    virtual void     close()                     = 0;
    // virtual void setsockopt() = 0;

   protected:
    int                sockfd_;            /**< Server socket file descriptor */
    struct sockaddr_in addr_in_;           /**< Server address */
    SessionGenerator   session_generator_; /**< Session generator function */
};

/**
 * @brief Represents a TCP socket
 */
class TcpSocket : public Socket {
   public:
    /**
     * @brief Construct a new TcpSocket object
     */
    TcpSocket(SessionGenerator session_generator);

   private:
    // TcpSocket(const TcpSocket& other);
    // TcpSocket& operator=(const TcpSocket& other);

   public:
    /**
     * @brief Destroy the TcpSocket object
     */
    ~TcpSocket();

    /**
     * @brief Bind the socket to a port and address
     *
     * @param addr address to bind to
     * @param port port to bind to
     */
    int bind(string addr, int port);

    /**
     * @brief Listen for connections
     */
    void listen();

    /**
     * @brief Accept a connection and connects to it
     */
    Session* accept();

    /**
     * @brief Close the socket
     */
    void close();
};

/** TcpSocket generator function */
Socket* tcp_socket_generator();
