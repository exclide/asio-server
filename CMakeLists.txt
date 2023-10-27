cmake_minimum_required(VERSION 3.25)
project(asio_server)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(asio_server main.cpp ChatServer.cpp ChatSession.cpp ChatRoom.cpp)
include_directories(asio_server ${Boost_INCLUDE_DIRS})
target_link_libraries(asio_server ws2_32 wsock32 OpenSSL::SSL)