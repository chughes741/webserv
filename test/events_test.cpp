
#include "events.hpp"

#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "webserv.hpp"
#include "events.hpp"

using namespace std;

class MockSocket {
   public:
    MOCK_METHOD4(send,
                 ssize_t(const void* buf, size_t len, int flags, int sockfd));
    int fd() const { return sockfd_; }

   private:
    int sockfd_;
};

TEST(writeResponseTest, StatusLineIsCorrect) {
    Response response;
    response.version = "HTTP/1.1";
    response.status  = "200 OK";
    response.server  = "TestServer/1.0";
    response.body    = "Hello, World!";

    testing::StrictMock<MockSocket> mockSocket;
    EXPECT_CALL(mockSocket,
                send(testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Invoke(
            [](const void* buf, size_t len, int flags, int sockfd) {
                (void)flags;
                (void)sockfd;
                std::string str(static_cast<const char*>(buf), len);
                return str.find("HTTP/1.1 200 OK TestServer/1.0\r\n") !=
                               std::string::npos
                           ? len
                           : -1;
            }));

    writeResponse(mockSocket.fd(), response);
}
