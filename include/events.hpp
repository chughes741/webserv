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

typedef uint32_t KqueueEvent;
typedef uint32_t InternalEvent;

enum EventType : InternalEvent {
    NONE             = 0,
    READABLE         = 1,
    WRITABLE         = 2,
    ERROR_EVENT      = 4,
    SIGNAL_EVENT     = 8,
    CONNECT_EVENT    = 16,
    DISCONNECT_EVENT = 32
};

// Abstract base class for event handlers
class EventListener {
   public:
    virtual ~EventListener();

    virtual std::pair<int, InternalEvent> listen()                                    = 0;
    virtual bool                          registerEvent(int fd, InternalEvent events) = 0;
    virtual void                          unregisterEvent(int fd)                     = 0;
};

// Kqueue event handler
class KqueueEventListener : public EventListener {
   public:
    KqueueEventListener();

    std::pair<int, InternalEvent> listen();
    bool                          registerEvent(int fd, InternalEvent events);
    void                          unregisterEvent(int fd);

   private:
    int                                  queue_fd_;       // kqueue file descriptor
    struct timespec                      timeout_;        // timeout for kevent
    std::map<int, struct kevent>         events_;         // ident, event parameters
    std::map<KqueueEvent, InternalEvent> KqueueEventMap;  // map for converting events
};
