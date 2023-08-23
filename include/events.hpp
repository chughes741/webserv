#pragma once

#include <fcntl.h>
#include <stdint.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include <map>
#include <stdexcept>
#include <utility>

#include "logging.hpp"

#define READABLE         1
#define WRITABLE         2
#define ERROR_EVENT      4
#define CONNECT_EVENT    8
#define DISCONNECT_EVENT 16

typedef uint32_t KqueueEvent;
typedef uint32_t InternalEvent;

/**
 * @brief Abstract class for handling event
 */
class EventListener {
   public:
    virtual ~EventListener();

    virtual std::pair<int, InternalEvent> listen()                          = 0;
    virtual bool                          registerEvent(int fd, int events) = 0;
    virtual void                          unregisterEvent(int fd)           = 0;
};

/** Event map to convert KqueueEvents to internal events */

/**
 * @brief KqueueEventListener class for handling events, used on MacOS
 *
 * @details This class wraps around the kqueue system call and provides
 * a simple interface for adding and removing sockets from the kqueue.
 */
class KqueueEventListener : public EventListener {
   public:
    KqueueEventListener();

    std::pair<int, InternalEvent> listen();
    bool                          registerEvent(int fd, int events);
    void                          unregisterEvent(int fd);

   private:
    int                                  queue_fd_; /**< kqueue file descriptor */
    struct timespec                      timeout_;  /**< timeout for kevent */
    std::map<int, struct kevent>         events_;   /**< ident, event parameters */
    std::map<KqueueEvent, InternalEvent> KqueueEventMap;
};

