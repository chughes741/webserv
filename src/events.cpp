/**
 * @file events.cpp
 * @brief Defines classes for handling events
 *
 * This file contains the implementation of the EventListener,
 * KqueueEventListener, and EpollEventListener classes. The EventListener class
 * is an abstract base class that provides an interface for creating event
 * loops. The KqueueEventListener class inherits from the EventListener class
 * and implements an event loop that uses the kqueue system call. The
 * EpollEventListener class inherits from the EventListener class and implements
 * an event loop that uses the epoll system call.
 *
 * @note The KqueueEventListener class is only available on MacOS and the
 * EpollEventListener class is only available on Linux.
 *
 * @note This code is for educational purposes only and should not be used in
 * production environments without extensive testing and modification.
 *
 * @version 0.1
 * @date 2023-04-24
 * @authors
 *  - Francis L.
 *  - Marc-André L.
 *  - Cole H.
 */

#include "events.hpp"

EventListener::~EventListener() {}

KqueueEventListener::KqueueEventListener() {
    // Initialize timeout
    timeout_.tv_sec  = 0;
    timeout_.tv_nsec = 0;

    // Create a new kqueue
    KqueueEventMap[EVFILT_READ]   = READABLE;
    KqueueEventMap[EVFILT_WRITE]  = WRITABLE;
    KqueueEventMap[EVFILT_EXCEPT] = ERROR_EVENT;

    queue_fd_ = kqueue();

    // Check if kqueue was created successfully
    if (queue_fd_ == -1) {
        throw std::runtime_error("creating kequeue failed");
    }
}

std::pair<int, InternalEvent> KqueueEventListener::listen() {
    // Wait for events on the kqueue.
    struct kevent eventlist[1];
    uint32_t filter = 0;
    int           ret = kevent(queue_fd_, NULL, 0, eventlist, 1, &timeout_);

    // Check if event was received successfully
    if (ret == -1) {
        throw std::runtime_error("eventListen() failed");
    }
    filter = static_cast<uint32_t>(eventlist[0].filter); // Cast eventlist filter to uint32_t to match KqueueEventMap
    // Handle conversion from kqueue events to internal events
    InternalEvent event = 0; // Default is nothing happened I guess
    for (std::map<KqueueEvent, InternalEvent>::const_iterator it =
             KqueueEventMap.begin();
         it != KqueueEventMap.end(); ++it) {
        if (!(filter ^ it->first)) {
            event = it->second;
            break;
        }
    }

    /** @todo check if any other information needs to be returned */

    // EVFILT_WRITE returns when it's possible to write, data will contain the
    // amount of space left in the write buffer

    // Return fd and event
    return std::make_pair(eventlist[0].ident, event);
}

bool KqueueEventListener::registerEvent(int fd, int events) {
    // Handle conversion from internal events to kqueue filter
    KqueueEvent filter = 0;
    for (std::map<KqueueEvent, InternalEvent>::const_iterator it = KqueueEventMap.begin();
         it != KqueueEventMap.end(); ++it) {
        if (events & it->second) {
            filter |= it->first;
        }
    }

    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        Logger::instance().log("Error: Failed to set socket to non-blocking");
        // throw std::runtime_error("registerEvent() failed");
        return false;
    }

    // Set flags and fflags
    struct kevent event;
    u_short       flags  = EV_ADD | EV_CLEAR;
    u_int         fflags = 0;  /** @todo check if we need EVFILT_VNODE */
    int64_t       data   = 0;  // Shouldn't be needed

    // Initialize kevent structure.
    EV_SET(&event, fd, filter, flags, fflags, data, NULL);

    // Add event to the kqueue.
    int ret = kevent(queue_fd_, &event, 1, NULL, 0, NULL);

    // Check if event was added successfully
    if (ret == -1) {
        Logger::instance().log("Error: Failed to add event to kqueue");
        // throw std::runtime_error("registerEvent() failed");
        return false;
    }

    // Add event to the map of events.
    events_[fd] = event;

    return true;
}

void KqueueEventListener::unregisterEvent(int fd) {
    // Check if event exists.
    if (events_.find(fd) == events_.end()) {
        throw std::runtime_error("unregisterEvent() failed");
    }

    // Get event from the map of events.
    struct kevent event  = events_[fd];
    KqueueEvent   filter = event.filter;
    u_short       flags  = EV_DELETE;
    u_int         fflags = 0;
    int64_t       data   = 0;

    // Edit event.
    EV_SET(&event, fd, filter, flags, fflags, data, NULL);

    // Delete event from the kqueue.
    int ret = kevent(queue_fd_, &event, 1, NULL, 0, NULL);

    // Check if event was deleted successfully
    if (ret == -1) {
        throw std::runtime_error("removeSocket() failed");
    }

    // Delete event from the map of events.
    events_.erase(fd);
}
