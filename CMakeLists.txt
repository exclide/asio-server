cmake_minimum_required(VERSION 3.25)
project(asio_server)

set(CMAKE_CXX_STANDARD 20)

#boost
include_directories($ENV{BOOST_ROOT})
#add_definitions(-O3)

add_executable(asio_server main.cpp)
