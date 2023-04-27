cmake_minimum_required(VERSION 3.22.1)
project(webserv VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/d25e625364a2078c940da2cb1fcf098aa184fd9f.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_MAKE_PROGRAM make)
add_compile_options(-Wall -Werror -Wextra -ffast-math -ftree-vectorize)
add_compile_definitions(CONFIG_FILE="../config/server.conf")


file(GLOB SOURCES "src/*.cpp")

add_executable(webserv
  src/events.cpp
  src/main.cpp
  src/server.cpp
  src/socket.cpp
  src/parsing.cpp
)
target_include_directories(webserv PUBLIC
  ${PROJECT_SOURCE_DIR}/include/
)
set_target_properties(webserv PROPERTIES
  LINKER_LANGUAGE CXX
)


# Test executables
enable_testing()

# events_test
add_executable(events_test test/events_test.cpp)
target_link_libraries(
  events_test
  PUBLIC GTest::gtest_main
)
target_include_directories(events_test PUBLIC
  ${PROJECT_SOURCE_DIR}/include/
)
add_test(
  NAME events_test
  COMMAND $<TARGET_FILE:events_test>
)

# parsing_test
add_executable(parsing_test test/parsing_test.cpp)
target_link_libraries(
  parsing_test
  PUBLIC GTest::gtest_main
)
target_include_directories(parsing_test PUBLIC
  ${PROJECT_SOURCE_DIR}/include/
)
add_test(
  NAME parsing_test
  COMMAND $<TARGET_FILE:parsing_test>
)

# server_test
add_executable(server_test test/server_test.cpp)
target_link_libraries(
  server_test
  PUBLIC GTest::gtest_main
)
target_include_directories(server_test PUBLIC
  ${PROJECT_SOURCE_DIR}/include/
)
add_test(
  NAME server_test
  COMMAND $<TARGET_FILE:server_test>
)

# socket_test
add_executable(socket_test test/socket_test.cpp)
target_link_libraries(
  socket_test
  PUBLIC gtest_main gmock_main
)
target_include_directories(socket_test PUBLIC
  ${PROJECT_SOURCE_DIR}/include/
)
add_test(
  NAME socket_test
  COMMAND $<TARGET_FILE:socket_test>
)

# webserv_test
add_executable(webserv_test test/webserv_test.cpp)
target_link_libraries(
  webserv_test
  PUBLIC GTest::gtest_main
)
target_include_directories(webserv_test PUBLIC
  ${PROJECT_SOURCE_DIR}/include/
)
add_test(
  NAME webserv_test
  COMMAND $<TARGET_FILE:webserv_test>
)
