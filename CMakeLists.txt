cmake_minimum_required(VERSION 3.26)

project(asio_server)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

set(sources main.cpp ChatServer.cpp ChatSession.cpp ChatRoom.cpp)
set(libs OpenSSL::SSL)
if (WIN32)
    set(libs ${libs} ws2_32 wsock32)
endif()

add_executable(asio_server ${sources})
include_directories(asio_server ${Boost_INCLUDE_DIRS})
target_link_libraries(asio_server ${libs})
