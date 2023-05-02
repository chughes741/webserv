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

EventListener::~EventListener() {
}

#ifdef __APPLE__

KqueueEventListener::KqueueEventListener() {
    // Create a new kqueue
    KqueueEventMap[EVFILT_READ] = READABLE;
    KqueueEventMap[EVFILT_WRITE] = WRITABLE;
    KqueueEventMap[EVFILT_EXCEPT] = ERROR_EVENT;
    
    queue_fd_        = kqueue();
    timeout_.tv_sec  = 0;
    timeout_.tv_nsec = 0;

    // Check if kqueue was created successfully
    if (queue_fd_ == -1) {
        throw runtime_error("creating kequeue failed");
    }
}

pair<int, InternalEvent> KqueueEventListener::listen() {
    // Wait for events on the kqueue.
    struct kevent eventlist[1];
    int           ret = kevent(queue_fd_, NULL, 0, eventlist, 1, &timeout_);

    // Check if event was received successfully
    if (ret == -1) {
        throw runtime_error("eventListen() failed");
    }

    // Handle conversion from kqueue events to internal events
    InternalEvent event = 0;
    for (map<KqueueEvent, InternalEvent>::const_iterator it =
             KqueueEventMap.begin();
         it != KqueueEventMap.end(); ++it) {
        if (eventlist[0].filter & it->first) {
            event |= it->second;
            break;
        }
    }

    /** @todo check if any other information needs to be returned */

    // EVFILT_WRITE returns when it's possible to write, data will contain the
    // amount of space left in the write buffer

    // Return fd and event
    return make_pair(eventlist[0].ident, event);
}

void KqueueEventListener::registerEvent(int fd, int events) {
    // Handle conversion from internal events to kqueue filter
    KqueueEvent filter = 0;
    for (map<KqueueEvent, InternalEvent>::const_iterator it =
             KqueueEventMap.begin();
         it != KqueueEventMap.end(); ++it) {
        if (events & it->second) {
            filter |= it->first;
        }
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
        throw runtime_error("registerEvent() failed");
    }

    // Add event to the map of events.
    events_[fd] = event;
}

void KqueueEventListener::unregisterEvent(int fd) {
    // Check if event exists.
    if (events_.find(fd) == events_.end()) {
        throw runtime_error("unregisterEvent() failed");
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
        throw runtime_error("removeSocket() failed");
    }

    // Delete event from the map of events.
    events_.erase(fd);
}

#elif __linux__

EpollEventListener::EpollEventListener() {
    // Create a new epoll
    epoll_fd_ = epoll_create(1);  // 1 is ignored, can't be 0

    // Check if epoll was created successfully
    if (epoll_fd_ == -1) {
        throw runtime_error("creating epoll failed");
    }
}

pair<int, InternalEvent> EpollEventListener::listen() {
    // Get an event from epoll
    struct epoll_event events[1];
    int                ret = epoll_wait(epoll_fd_, events, 1, -1);

    // Check if events were received successfully
    if (ret == -1) {
        throw runtime_error("epoll_wait() failed");
    }

    // Handle conversion from epoll events to internal events
    InternalEvent event = 0;
    for (map<EpollEvent, InternalEvent>::const_iterator it =
             EpollEventMap.begin();
         it != EpollEventMap.end(); ++it) {
        if (it->first & events[0].events) {
            event |= it->second;
        }
    }

    // Return fd and event
    return make_pair(events[0].data.fd, event);
}

void EpollEventListener::registerEvent(int fd, int events) {
    struct epoll_event event; /** @todo does this need to be 0ed */

    // Initialize epoll_event structure.
    event.data.fd = fd;

    // Handle conversion from internal events to epoll events
    for (map<EpollEvent, InternalEvent>::const_iterator it =
             EpollEventMap.begin();
         it != EpollEventMap.end(); ++it) {
        if (it->second & events) {
            events |= it->first;
        }
    }

    // Add event to the epoll.
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);

    // Check if event was added successfully
    if (ret == -1) {
        throw runtime_error("registerEvent() failed");
    }

    // Add event to the map of events.
    events_[fd] = event;
}

void EpollEventListener::unregisterEvent(int fd) {
    // Check if event exists.
    if (events_.find(fd) == events_.end()) {
        throw runtime_error("unregisterEvent() failed");
    }

    // Delete event from the epoll.
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL);

    // Check if event was deleted successfully
    if (ret == -1) {
        throw runtime_error("unregisterEvent() failed");
    }

    // Delete event from the map of events.
    events_.erase(fd);
}

#endif  // __APPLE__