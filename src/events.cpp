#include "events.hpp"

KqueueEventListener::KqueueEventListener() {
    // Initialize timeout
    timeout_.tv_sec  = 0;
    timeout_.tv_nsec = 0;

    // Create a new kqueue
    KqueueEventMap[EVFILT_READ]   = READABLE;
    KqueueEventMap[EVFILT_WRITE]  = WRITABLE;
    KqueueEventMap[EVFILT_EXCEPT] = ERROR_EVENT;
    KqueueEventMap[EVFILT_SIGNAL] = SIGNAL_EVENT;

    queue_fd_ = kqueue();

    // Check if kqueue was created successfully
    if (queue_fd_ == -1) {
        throw std::runtime_error("Failed to create kqueue");
    }
}

std::pair<int, InternalEvent> KqueueEventListener::listen() {
    // Wait for events on the kqueue.
    struct kevent eventlist;
    KqueueEvent   filter = 0;
    int           ret    = kevent(queue_fd_, NULL, 0, &eventlist, 1, &timeout_);

    // Check if event was received successfully
    if (ret == -1) {
        Logger::instance().log("Error: Failed to receive event from kqueue");
        return std::make_pair(-1, NONE);
    }

    filter = static_cast<KqueueEvent>(eventlist.filter);

    // Handle conversion from kqueue events to internal events
    InternalEvent event = NONE;
    for (std::map<KqueueEvent, InternalEvent>::const_iterator it = KqueueEventMap.begin();
         it != KqueueEventMap.end(); ++it) {
        if (!(filter ^ it->first)) {
            event = it->second;
            break;
        }
    }

    // Return fd and event
    return std::make_pair(eventlist.ident, event);
}

bool KqueueEventListener::registerEvent(int fd, InternalEvent events) {
     std::cout << "INTERNAL EVENT HERE IS: " << events << std::endl;
     if (events == SIGNAL_EVENT) {
        Logger::instance().log("Registering signal event");  
     } else {
        Logger::instance().log("Registering event: " + std::to_string(events) +
                          " on fd: " + std::to_string(fd));
     }

    // Handle conversion from internal events to kqueue filter
    KqueueEvent filter = NONE;
    for (std::map<KqueueEvent, InternalEvent>::const_iterator it = KqueueEventMap.begin();
         it != KqueueEventMap.end(); ++it) {
        if (events & it->second) {
            filter |= it->first;
        }
    }

    if (events != SIGNAL_EVENT && fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        Logger::instance().log("Error: Failed to set socket to non-blocking");
        return false;
    }

    // Set flags and fflags
    struct kevent event;
    u_short       flags  = EV_ADD | EV_CLEAR;
    u_int         fflags = 0;  /** @todo check if we need EVFILT_VNODE */
    int64_t       data   = 0;  // Shouldn't be needed

    // Initialize kevent structure.
    if (events == 0) {
        Logger::instance().log("Error: No events specified during registerEvent()");
        return false;
    }
    if (events & SIGNAL_EVENT) {
        EV_SET(&event, fd, EVFILT_SIGNAL, flags, fflags, data, NULL);
    } else {
        EV_SET(&event, fd, filter, flags, fflags, data, NULL);
    }

    // Add event to the kqueue.
    if (kevent(queue_fd_, &event, 1, NULL, 0, NULL) == -1) {
        Logger::instance().log("Error: Failed to add event to kqueue");
        return false;
    }

    // Add event to the map of events.
    events_[fd] = event;

    return true;
}

void KqueueEventListener::unregisterEvent(int fd, InternalEvent events) {
    // Logger::instance().log("Unregistering events on fd: " + std::to_string(fd));

    // Check if event exists.
    if (events_.find(fd) == events_.end()) {
        Logger::instance().log("Error: Event does not exist during unregisterEvent()");
        return;
    }

    // Handle conversion from internal events to kqueue filter
    KqueueEvent filter = NONE;
    for (std::map<KqueueEvent, InternalEvent>::const_iterator it = KqueueEventMap.begin();
         it != KqueueEventMap.end(); ++it) {
        if (events & it->second) {
            filter |= it->first;
        }
    }

    // Get event from the map of events.
    struct kevent event  = events_[fd];
    u_short       flags  = EV_DELETE;
    u_int         fflags = 0;
    int64_t       data   = 0;

    // Edit event.
    EV_SET(&event, fd, filter, flags, fflags, data, NULL);

    // Delete event from the kqueue.
    if (kevent(queue_fd_, &event, 1, NULL, 0, NULL) == -1) {
        Logger::instance().log("Error: Failed to remove event from kqueue");
    }
}

void KqueueEventListener::removeEvent(int fd) {
    events_.erase(fd);
}