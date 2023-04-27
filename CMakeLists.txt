cmake_minimum_required(VERSION 3.22.1)
project(webserv VERSION 1.0.0)

# Dependencies
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/d25e625364a2078c940da2cb1fcf098aa184fd9f.zip
)
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Global options
set(CMAKE_CXX_STANDARD 98)
add_compile_options(-Wall -Werror -Wextra)
option(OPTIMIZE "Enable compiler optimizations" ON)
if(OPTIMIZE)
  add_compile_options(-ffast-math -ftree-vectorize -O3)
endif()

# Main executable
file(GLOB SOURCES "src/*.cpp")
add_executable(webserv ${SOURCES})
target_include_directories(webserv PUBLIC ${PROJECT_SOURCE_DIR}/include/)
target_link_options(webserv PRIVATE -lstdc++)
set_target_properties(webserv PROPERTIES LINKER_LANGUAGE CXX)

# Test targets
enable_testing()

add_executable(events_unit_tests test/events_test.cpp ${SOURCES})
target_link_libraries(events_unit_tests PUBLIC GTest::gtest_main
                                               GTest::gmock_main)
target_include_directories(events_unit_tests
                           PUBLIC ${PROJECT_SOURCE_DIR}/include/)
add_test(NAME events_unit_tests COMMAND $<TARGET_FILE:events_unit_tests>)

add_executable(parsing_unit_tests test/parsing_test.cpp ${SOURCES})
target_link_libraries(parsing_unit_tests PUBLIC GTest::gtest_main
                                                GTest::gmock_main)
target_include_directories(parsing_unit_tests
                           PUBLIC ${PROJECT_SOURCE_DIR}/include/)
add_test(NAME parsing_unit_tests COMMAND $<TARGET_FILE:parsing_unit_tests>)

add_executable(server_unit_tests test/server_test.cpp ${SOURCES})
target_link_libraries(server_unit_tests PUBLIC GTest::gtest_main
                                               GTest::gmock_main)
target_include_directories(server_unit_tests
                           PUBLIC ${PROJECT_SOURCE_DIR}/include/)
add_test(NAME server_unit_tests COMMAND $<TARGET_FILE:server_unit_tests>)

add_executable(socket_unit_tests test/socket_test.cpp ${SOURCES})
target_link_libraries(socket_unit_tests PUBLIC GTest::gtest_main
                                               GTest::gmock_main)
target_include_directories(socket_unit_tests
                           PUBLIC ${PROJECT_SOURCE_DIR}/include/)
add_test(NAME socket_unit_tests COMMAND $<TARGET_FILE:socket_unit_tests>)

add_executable(webserv_unit_tests test/webserv_test.cpp ${SOURCES})
target_link_libraries(webserv_unit_tests PUBLIC GTest::gtest_main
                                                GTest::gmock_main)
target_include_directories(webserv_unit_tests
                           PUBLIC ${PROJECT_SOURCE_DIR}/include/)
add_test(NAME webserv_unit_tests COMMAND $<TARGET_FILE:webserv_unit_tests>)
