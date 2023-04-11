
#include "webserv.hpp"
#include <gtest/gtest.h>

TEST(webserv, trying) {
	EXPECT_EQ(1, 1);
}

TEST(WebServerTest, TestNginxConfig) {
  // Define the path to your Nginx configuration file
  char buf[PATH_MAX];
  if (getcwd(buf, sizeof(buf)) == NULL) {
	std::cerr << "Error retrieving current working directory" << std::endl;
	exit(EXIT_FAILURE);
  }
  std::string config_file_path = std::string(buf) + "/webserv.conf";

  // Run the `nginx -T` command to print the parsed configuration file to stdout
  std::string nginx_cmd = "nginx -T -c " + config_file_path;
  FILE* pipe = popen(nginx_cmd.c_str(), "r");
  if (!pipe) {
    std::cerr << "Error running command: " << nginx_cmd << std::endl;
    exit(EXIT_FAILURE);
  }
  char buffer[4096];
  std::string nginx_output = "";
  while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
    nginx_output += buffer;
  }
  pclose(pipe);

  // Load the same configuration file in your web server program
  // and capture the output to a string
  std::string server_cmd = "./webserv " + config_file_path;
  pipe = popen(server_cmd.c_str(), "r");
  if (!pipe) {
    std::cerr << "Error running command: " << server_cmd << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string server_output = "";
  while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
    server_output += buffer;
  }
  pclose(pipe);

  // Compare the Nginx output to your web server program output
  ASSERT_EQ(nginx_output, server_output);
}