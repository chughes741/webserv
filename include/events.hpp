/**
 * @file events.hpp
 * @brief Declares classes for handling events on sockets using the kqueue or
 * epoll system calls.
 *
 * This file contains the declaration of the abstract EventListener class and
 * its two derived classes, KqueueEventListener and EpollEventListener. The
 * EventListener class provides an interface for handling events on sockets,
 * such as registerEventing and unregisterEventing sockets for read and write
 * events. The KqueueEventListener and EpollEventListener classes inherit from
 * the EventListener class and implement the event handling functionality using
 * the kqueue and epoll system calls, respectively.
 *
 * To use the EventListener classes, you must create an instance of one of the
 * derived classes, registerEvent the sockets you want to monitor using the
 * registerEventSocket() method, and then call the start() method to begin
 * monitoring events on the registerEvented sockets. Once you have received an
 * event, you can use the handleEvent() method to process the event and take
 * appropriate action, such as reading or writing data to the socket.
 *
 * @note The KqueueEventListener class is used on MacOS, and the
 * EpollEventListener class is used on Linux.
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

#include <stdexcept>
#include <map>

using std::map;
using std::runtime_error;

/**
 * @brief Abstract class for handling event
 */
class EventListener {
   public:
    virtual ~EventListener();

    virtual int  listen()                          = 0;
    virtual void registerEvent(int fd, int events) = 0;
    virtual void unregisterEvent(int fd)           = 0;

   protected:
};

#ifdef __APPLE__
/**
 * @brief KqueueEventListener class for handling events, used on MacOS
 *
 * @details This class wraps around the kqueue system call and provides
 * a simple interface for adding and removing sockets from the kqueue.
 */
class KqueueEventListener : public EventListener {
   public:
    KqueueEventListener();

    int  listen();
    void registerEvent(int fd, int events);
    void unregisterEvent(int fd);

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
class EpollEventListener : public EventListener {
   public:
    EpollEventListener();

    int  listen();
    void registerEvent(int fd, int events);
    void unregisterEvent(int fd);

   private:
    int                          epoll_fd_; /**< epoll file descriptor */
    map<int, struct epoll_event> events_;   /**< ident, event parameters */
};
#endif  // __APPLE__
