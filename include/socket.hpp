#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>

#include "webserv.hpp"

#define SO_MAX_QUEUE     10
#define READ_BUFFER_SIZE 1024
#define CRLF             "\r\n"

// Session abstract base class
class Session {
   public:
    Session(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    virtual ~Session() = 0;

    virtual void                            send(int client)       = 0;
    virtual std::pair<std::string, ssize_t> recv(int client) const = 0;
    int                                     getSockFd() const;
    void                                    addSendQueue(const std::string& buffer);

   protected:
    int                     sockfd_;     /**< Session socket file descriptor */
    const struct sockaddr*  addr_;       /**< Session socket address */
    socklen_t               addrlen_;    /**< Session socket address length */
    std::queue<std::string> send_queue_; /**< Queue of messages to send */
};

// TcpSession class
class TcpSession : public Session {
   public:
    TcpSession(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

    void                            send(int client);
    std::pair<std::string, ssize_t> recv(int client) const;
};

// TcpSession generator function
Session* tcp_session_generator(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

// Socket abstract base class
class Socket {
   public:
    typedef Session* (*SessionGenerator)(int sockfd, const struct sockaddr* addr,
                                         socklen_t addrlen);

    Socket(SessionGenerator session_generator);
    virtual ~Socket() = 0;

    virtual int      bind(std::string addr, int port) = 0;
    virtual void     listen()                         = 0;
    virtual Session* accept()                         = 0;
    virtual void     close()                          = 0;
    // virtual void setsockopt() = 0;

   protected:
    int                sockfd_;            /**< Server socket file descriptor */
    struct sockaddr_in addr_in_;           /**< Server address */
    SessionGenerator   session_generator_; /**< Session generator function */
};

// TcpSocket class
class TcpSocket : public Socket {
   public:
    TcpSocket(SessionGenerator session_generator = tcp_session_generator);

   private:
    // TcpSocket(const TcpSocket& other);
    // TcpSocket& operator=(const TcpSocket& other);

   public:
    ~TcpSocket();

    int      bind(std::string addr, int port);
    void     listen();
    Session* accept();
    void     close();
};

// TcpSocket generator function
Socket* tcp_socket_generator();
