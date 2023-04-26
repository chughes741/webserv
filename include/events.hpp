/**
 * @file events.hpp
 * @brief Declares classes for handling events on sockets using the kqueue or
 * epoll system calls.
 * 
 * This file contains the declaration of the abstract EventLoop class and its
 * two derived classes, KqueueEventLoop and EpollEventLoop. The EventLoop class
 * provides an interface for handling events on sockets, such as registering
 * and unregistering sockets for read and write events. The KqueueEventLoop and
 * EpollEventLoop classes inherit from the EventLoop class and implement the
 * event handling functionality using the kqueue and epoll system calls,
 * respectively.
 * 
 * To use the EventLoop classes, you must create an instance of one of the
 * derived classes, register the sockets you want to monitor using the
 * registerSocket() method, and then call the start() method to begin monitoring
 * events on the registered sockets. Once you have received an event, you can
 * use the handleEvent() method to process the event and take appropriate
 * action, such as reading or writing data to the socket.
 * 
 * @note The KqueueEventLoop class is used on MacOS, and the EpollEventLoop
 * class is used on Linux.
 * 
 * @note This code is for educational purposes only and should not be used in
 * production environments without extensive testing and modification.
 * 
 * @version 0.1
 * @date 2023-04-24
 * @authors
 *   - Francis L.
 *   - Marc-André L.
 *   - Cole H.
 */

#pragma once

#ifdef __APPLE__
#include <sys/event.h>
#elif __linux__
#include <sys/epoll.h>
#endif  // __APPLE__
#include <sys/time.h>
#include <sys/types.h>

#include <map>
#include <exception>

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
