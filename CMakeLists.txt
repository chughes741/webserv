
cmake_minimum_required(VERSION 3.22.1)
# set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
#   CACHE STRING "Vcpkg toolchain file")
project(webserv VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_MAKE_PROGRAM make)

# find_package(GTest CONFIG REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/include/
  ${PROJECT_SOURCE_DIR}/src/
)

file(GLOB all_SRCS
  ${PROJECT_SOURCE_DIR}/include/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(webserv ${all_SRCS})

# Test executables
# enable_testing()

# add_test(
  # NAME webserv_test
  # COMMAND $<TARGET_FILE:webserv_test>
# )
