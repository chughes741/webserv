#pragma once

#ifdef __APPLE__
#include <sys/event.h>
#elif __linux__
#include <sys/epoll.h>
#endif  // __APPLE__
#include <sys/time.h>
#include <sys/types.h>

#include "webserv.hpp"

using std::map;
using std::runtime_error;

/**
 * @brief Abstract class for handling event
 */
class EventLoop {
   public:
    virtual ~EventLoop();

    virtual void start()                            = 0;
    virtual void stop()                             = 0;
    virtual void registerSocket(int fd, int events) = 0;
    virtual void unregisterSocket(int fd)           = 0;

   protected:
};

#ifdef __APPLE__
/**
 * @brief KqueueEventLoop class for handling events, used on MacOS
 *
 * @details This class wraps around the kqueue system call and provides
 * a simple interface for adding and removing sockets from the kqueue.
 */
class KqueueEventLoop : public EventLoop {
   public:
    KqueueEventLoop();

    void start();
    void stop();
    void registerSocket(int fd, int events);
    void unregisterSocket(int fd);

   private:
    struct kevent eventListen();

   private:
    int                     queue_fd_; /**< kqueue file descriptor */
    struct timespec         timeout_;  /**< timeout for kevent */
    map<int, struct kevent> events_;   /**< ident, event parameters */
};
#elif __linux__
/**
 * @brief Epoll class for handling events, used on Linux
 *
 * @details This class wraps around the epoll system call and provides
 * a simple interface for adding and removing sockets from the epoll.
 */
class EpollEventLoop : public EventLoop {
   public:
    EpollEventLoop();

    void start();
    void stop();
    void registerSocket(int fd, int events);
    void unregisterSocket(int fd);

   private:
    int                          epoll_fd_; /**< epoll file descriptor */
    map<int, struct epoll_event> events_;   /**< ident, event parameters */
};
#endif  // __APPLE__
