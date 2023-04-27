#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "webserv.hpp"

class MockSocket : public Socket {
   public:
    MOCK_METHOD(int, bind, (string addr, int port), (override));
    MOCK_METHOD(void, listen, (), (override));
    MOCK_METHOD(pair<int, Session>, accept, (), (override));
    MOCK_METHOD(void, close, (), (override));
};

TEST(serverTest, BasicAssertions) {
    EXPECT_TRUE(true);
}
