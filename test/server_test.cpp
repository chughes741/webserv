#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <utility>

#include "webserv.hpp"

using std::pair;

using ::testing::AtLeast;
using ::testing::NiceMock;

class MockSocket : public Socket {
   public:
    MOCK_METHOD(int, bind, (string addr, int port), (override));
    MOCK_METHOD(void, listen, (), (override));
    MOCK_METHOD((pair<int, Session>), accept, (), (override));
    MOCK_METHOD(void, close, (), (override));
};

class MockEventListener : public EventListener {
   public:
    MOCK_METHOD(int, listen, (), (override));
    MOCK_METHOD(void, registerEvent, (int fd, int events), (override));
    MOCK_METHOD(void, unregisterEvent, (int fd), (override));
};

Socket *mock_socket_generator() {
    return new MockSocket();
}

TEST(ServerTest, StartAndStop) {
    // Create a default HttpConfig
    HttpConfig httpConfig;

    // Create a nice mock for Socket
    NiceMock<MockSocket> mockSocket;

    // Create a nice mock for EventListener
    NiceMock<MockEventListener> mockEventListener;

    // Create a Server object and pass in the mock Socket and EventListener
    HttpServer server(mock_socket_generator, httpConfig,
                      &mockEventListener);

    // Set up expectations for the mock EventListener
    EXPECT_CALL(mockEventListener, listen())
        .Times(1)
        .WillOnce(testing::Return(0));
    EXPECT_CALL(mockEventListener, registerEvent(testing::_, testing::_))
        .Times(AtLeast(1));
    EXPECT_CALL(mockEventListener, unregisterEvent(testing::_))
        .Times(AtLeast(1));

    // Call the start() method of the Server object
    server.start(false);

    // Call the stop() method of the Server object
    server.stop();
}
