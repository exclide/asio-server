cmake_minimum_required(VERSION 3.26)

project(asio_server)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(sources main.cpp ChatServer.cpp ChatSession.cpp ChatRoom.cpp
        UserRepository.h
        sha256.h
        User.h
        AuthService.h
        HttpSession.h
)
set(libs OpenSSL::SSL nlohmann_json::nlohmann_json)

add_executable(asio_server ${sources})
include_directories(asio_server ${Boost_INCLUDE_DIRS})
target_link_libraries(asio_server ${libs})
