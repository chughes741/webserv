/**
 * @file events.cpp
 * @brief Defines classes for handling events
 *
 * This file contains the implementation of the EventLoop, KqueueEventLoop, and
 * EpollEventLoop classes. The EventLoop class is an abstract base class that
 * provides an interface for creating event loops. The KqueueEventLoop class
 * inherits from the EventLoop class and implements an event loop that uses the
 * kqueue system call. The EpollEventLoop class inherits from the EventLoop
 * class and implements an event loop that uses the epoll system call.
 *
 * @note The KqueueEventLoop class is only available on MacOS and the
 * EpollEventLoop class is only available on Linux.
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

EventLoop::~EventLoop() {
}

#ifdef __APPLE__

KqueueEventLoop::KqueueEventLoop() {
    // Create a new kqueue
    queue_fd_ = kqueue();
    timeout_.tv_sec = 0;
    timeout_.tv_nsec = 0;

    // Check if kqueue was created successfully
    if (queue_fd_ == -1) {
        throw runtime_error("creating kequeue failed");
    }
}

int KqueueEventLoop::wait() {
    // Wait for events on the kqueue.
    struct kevent event;
    int           ret = kevent(queue_fd_, NULL, 0, &event, 1, &timeout_);

    // Check if event was received successfully
    if (ret == -1) {
        throw runtime_error("eventListen() failed");
    }

    return event.ident;
}

void KqueueEventLoop::registerSocket(int fd, int events) {
    struct kevent event;

    // Handle conversion from events to kqueue events
    (void)events;

    // Initialize kevent structure.
    EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);

    // Add event to the kqueue.
    int ret = kevent(queue_fd_, &event, 1, NULL, 0, NULL);

    // Check if event was added successfully
    if (ret == -1) {
        throw runtime_error("registerSocket() failed");
    }

    // Add event to the map of events.
    events_[fd] = event;
}

void KqueueEventLoop::unregisterSocket(int fd) {
    // Check if event exists.
    if (events_.find(fd) == events_.end()) {
        throw runtime_error("unregisterSocket() failed");
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

EpollEventLoop::EpollEventLoop() {
    // Create a new epoll
    epoll_fd_ = epoll_create(1);  // 1 is ignored, can't be 0

    // Check if epoll was created successfully
    if (epoll_fd_ == -1) {
        throw runtime_error("creating epoll failed");
    }
}

int EpollEventLoop::wait() {
    // Get an event from epoll
    struct epoll_event events[1];
    int                ret = epoll_wait(epoll_fd_, events, 1, -1);

    // Check if events were received successfully
    if (ret == -1) {
        throw runtime_error("epoll_wait() failed");
    }

    return events[0].data.fd;
}

void EpollEventLoop::registerSocket(int fd, int events) {
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
        throw runtime_error("registerSocket() failed");
    }

    // Add event to the map of events.
    events_[fd] = event;
}

void EpollEventLoop::unregisterSocket(int fd) {
    // Check if event exists.
    if (events_.find(fd) == events_.end()) {
        throw runtime_error("unregisterSocket() failed");
    }

    // Delete event from the epoll.
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL);

    // Check if event was deleted successfully
    if (ret == -1) {
        throw runtime_error("unregisterSocket() failed");
    }

    // Delete event from the map of events.
    events_.erase(fd);
}

#endif  // __APPLE__