/**
 * @file events.cpp
 * @brief Defines classes for handling events
 *
 * This file contains the implementation of the EventListener, KqueueEventListener, and
 * EpollEventListener classes. The EventListener class is an abstract base class that
 * provides an interface for creating event loops. The KqueueEventListener class
 * inherits from the EventListener class and implements an event loop that uses the
 * kqueue system call. The EpollEventListener class inherits from the EventListener
 * class and implements an event loop that uses the epoll system call.
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
    queue_fd_ = kqueue();
    timeout_.tv_sec = 0;
    timeout_.tv_nsec = 0;

    // Check if kqueue was created successfully
    if (queue_fd_ == -1) {
        throw runtime_error("creating kequeue failed");
    }
}

int KqueueEventListener::listen() {
    // Wait for events on the kqueue.
    struct kevent event;
    int           ret = kevent(queue_fd_, NULL, 0, &event, 1, &timeout_);

    // Check if event was received successfully
    if (ret == -1) {
        throw runtime_error("eventListen() failed");
    }

    return event.ident;
}

void KqueueEventListener::registerEvent(int fd, int events) {
    struct kevent event;

    // Handle conversion from events to kqueue events
    (void)events;

    // Initialize kevent structure.
    EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);

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
    struct kevent event = events_[fd];

    // Edit event.
    EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);

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

int EpollEventListener::listen() {
    // Get an event from epoll
    struct epoll_event events[1];
    int                ret = epoll_wait(epoll_fd_, events, 1, -1);

    // Check if events were received successfully
    if (ret == -1) {
        throw runtime_error("epoll_wait() failed");
    }

    return events[0].data.fd;
}

void EpollEventListener::registerEvent(int fd, int events) {
    struct epoll_event event;

    // Handle conversion from events to epoll events
    (void)events;

    // Initialize epoll_event structure.
    event.events  = events;
    event.data.fd = fd;

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