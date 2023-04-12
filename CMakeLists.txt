# If cmake doesnt work, update cmake path in settings to the value of : which cmake

cmake_minimum_required(VERSION 3.22.1)
# set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
#   CACHE STRING "Vcpkg toolchain file")
project(webserv VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_MAKE_PROGRAM make)
add_compile_options(-Wall -Werror -Wextra -ffast-math -ftree-vectorize)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true #To get rid of the warning
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# find_package(GTest CONFIG REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/include/
  ${PROJECT_SOURCE_DIR}/src/
)

file(GLOB all_SRCS
  ${PROJECT_SOURCE_DIR}/include/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(
	ft_STL
	SHARED
	include/webserv.hpp
)
set_target_properties(
	ft_STL
	PROPERTIES LINKER_LANGUAGE CXX
)

file(GLOB all_SRCS
  ${PROJECT_SOURCE_DIR}/include/*.hpp
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(webserv ${all_SRCS})

# Test executables
enable_testing()
add_executable(webserv_test test/webserv_test.cpp)
target_link_libraries(webserv_test PUBLIC ft_STL GTest::gtest_main)
add_test(
  NAME webserv_test
  COMMAND $<TARGET_FILE:webserv_test>
)
