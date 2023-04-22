#include "events.hpp"

// KQueue static member initialization
int KQueue::queue_fd_ = -1;
struct timespec KQueue::timeout_ = {0, 0};
map<int, struct kevent> KQueue::events_;


void KQueue::createQueue() {
	// Creates kqueue. Does not run if one already exists
	if (queue_fd_ == 0) {
		queue_fd_ = kqueue();
	}

	// Check if kqueue was created successfully
	if (queue_fd_ == -1) {
		throw runtime_error("createQueue() failed");
	}
}

void KQueue::addSocket(int fd) {
	struct kevent event;

	// Initialize kevent structure.
	EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
	
	// Add event to the kqueue.
	int ret = kevent(queue_fd_, &event, 1, NULL, 0, NULL);

	// Check if event was added successfully
	if (ret == -1) {
		throw runtime_error("addSocket() failed");
	}

	// Add event to the map of events.
	events_[fd] = event;
}

void KQueue::removeSocket(int fd) {
	// Check if event exists.
	if (events_.find(fd) == events_.end()) {
		throw runtime_error("removeSocket() failed");
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

struct kevent KQueue::eventListen() {
	// Wait for events on the kqueue.
	struct kevent event;
	int ret = kevent(queue_fd_, NULL, 0, &event, 1, &timeout_);

	// Check if event was received successfully
	if (ret == -1) {
		throw runtime_error("eventListen() failed");
	}

	return event;
}
