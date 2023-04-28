#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "webserv.hpp"
#include "parsing.hpp"
#include "config.hpp"

TEST(parsingTest, GlobalErrorLog) {
	HttpConfig httpConfig;
	parseConfig("../test/parsing_test.conf", httpConfig);
	EXPECT_EQ(httpConfig.error_log, "logs/error.log");
}

TEST(parsingTest, GlobalWorkerConnections) {
	HttpConfig httpConfig;
	parseConfig("../test/parsing_test.conf", httpConfig);
	EXPECT_EQ(httpConfig.error_log, "logs/error.log");
}