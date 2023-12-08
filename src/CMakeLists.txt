cmake_minimum_required(VERSION 3.26)

project(asio_server)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")


set(sources
        main.cpp
        ChatServer.cpp
        HttpSession.cpp
        WebsocketSession.cpp
        ChatRoom.cpp
        MessageService.cpp
        AuthService.cpp
        Sha256.cpp
        ChatException.h
        DbConnPool.h
        DbConfigParser.h
        ParseException.h
        DbConfig.h
        DbConnPool.cpp
        HttpHandler.h
        MessageRepository.cpp
        UserRepository.cpp
        HttpHandler.cpp
        IMessageRepository.h
        IUserRepository.h
        ../test/StubMessageRepository.h
)
set(libs OpenSSL::SSL nlohmann_json::nlohmann_json libpqxx::pqxx)


add_executable(asio_server ${sources})
include_directories(asio_server ${Boost_INCLUDE_DIRS} ${JWT_CPP_INCLUDE_DIRS})
target_link_libraries(asio_server ${libs})
