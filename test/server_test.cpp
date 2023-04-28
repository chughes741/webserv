#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <utility>

#include "webserv.hpp"

using std::pair;

using ::testing::_;
using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::Return;

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

Socket* mock_socket_generator() {
    static MockSocket* mockSocket = new MockSocket();
    return mockSocket;
}

TEST(ServerTest, StartAndStop) {
    // Create a default HttpConfig
    HttpConfig httpConfig;

    // Create a nice mock for EventListener
    NiceMock<MockEventListener> mockEventListener;

    MockSocket* mockSocket = dynamic_cast<MockSocket*>(mock_socket_generator());

    // Create a Server object and pass in the mock Socket and EventListener
    HttpServer server(mock_socket_generator, httpConfig, &mockEventListener);

    // Set up expectations for the mock Socket
    EXPECT_CALL(*mockSocket, bind(_, _))
        .Times(AtLeast(1));
    EXPECT_CALL(*mockSocket, listen())
        .Times(AtLeast(1));
    EXPECT_CALL(*mockSocket, close())
        .Times(AtLeast(1));


    // Set up expectations for the mock EventListener
    EXPECT_CALL(mockEventListener, registerEvent(_, _)).Times(AtLeast(1));

    // Call the start() method of the Server object
    server.start(false);

    // Call the stop() method of the Server object
    server.stop();
}
