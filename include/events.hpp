#pragma once

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include <map>
#include <exception>

using std::map;
using std::runtime_error;

/**
 * @brief KQueue class for handling events
 *
 * @details This class wraps around the kqueue system call and provides
 * a simple interface for adding and removing sockets from the kqueue.
 */
class KQueue {
   public:
    /**
     * @brief Creates kqueue. Does not run if one already exists
     */
    static void createQueue();

    /**
     * @brief Adds a socket to the kqueue
     *
     * @param fd file descriptor of the socket
     */
    static void addSocket(int fd);

    /**
     * @brief Removes a socket from the kqueue
     *
     * @param fd file descriptor of the socket
     */
    static void removeSocket(int fd);

    /**
     * @brief Waits for events on the kqueue
     */
    static struct kevent eventListen();

   private:
    static int                     queue_fd_; /**< kqueue file descriptor */
    static struct timespec         timeout_;  /**< timeout for kevent */
    static map<int, struct kevent> events_;   /**< ident, event parameters */
};
