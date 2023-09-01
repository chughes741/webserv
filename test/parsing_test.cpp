#include "parsing.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "config.hpp"
#include "parsing.hpp"

#define TEST_CONFIG "../test/parsing_test.conf"

TEST(parsingTest, GlobalUser) {
    HttpConfig httpConfig;
    parseConfig(TEST_CONFIG, httpConfig);
    EXPECT_EQ(httpConfig.user, "nobody");
}

TEST(parsingTest, GlobalWorkerProcesses) {
    HttpConfig httpConfig;
    parseConfig(TEST_CONFIG, httpConfig);
    EXPECT_EQ(httpConfig.worker_processes, 1);
}

TEST(parsingTest, GlobalErrorLog) {
    HttpConfig httpConfig;
    parseConfig(TEST_CONFIG, httpConfig);
    EXPECT_EQ(httpConfig.error_log, "logs/error.log");
}

TEST(parsingTest, EventsWorkerConnections) {
    HttpConfig httpConfig;
    parseConfig(TEST_CONFIG, httpConfig);
    EXPECT_EQ(httpConfig.worker_connections, 2048);
}

TEST(parsingTest, LocationRoot) {
    HttpConfig httpConfig;
    parseConfig(TEST_CONFIG, httpConfig);
    EXPECT_EQ(httpConfig.servers[0].locations["/"].root, "html");
}

TEST(parsingTest, LocationDiffRoot) {
    HttpConfig httpConfig;
    parseConfig(TEST_CONFIG, httpConfig);
    EXPECT_EQ(httpConfig.servers[1].locations["/"].root, "pas-html");
}

TEST(parsingTest, ServerName) {
    HttpConfig httpConfig;
    parseConfig(TEST_CONFIG, httpConfig);
    EXPECT_EQ(httpConfig.servers[2].server_names[0], "localhost");
}

TEST(parsingTest, ServerErrorPage) {
    HttpConfig httpConfig;
    parseConfig(TEST_CONFIG, httpConfig);
    EXPECT_EQ(httpConfig.servers[0].error_page[500], "/500.html");
}

TEST(parsingTest, HttpErrorPage) {
    HttpConfig httpConfig;
    parseConfig(TEST_CONFIG, httpConfig);
    EXPECT_EQ(httpConfig.error_page[500], "/500.html");
}