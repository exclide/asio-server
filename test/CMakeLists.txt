CMAKE_MINIMUM_REQUIRED(VERSION 3.26)

set(CMAKE_CXX_STANDARD 17)

project(server_test)

find_package(GTest CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

set(sources
        main.cpp
        ../src/Sha256.cpp
        ../src/MessageService.cpp
        ../src/AuthService.cpp
        ../src/HttpHandler.cpp
        StubUserRepository.h
        StubUserRepository.cpp
        StubMessageRepository.cpp)
set(libs OpenSSL::SSL GTest::gtest GTest::gtest_main)


add_executable(server_test ${sources})
target_link_libraries(server_test ${libs})

add_test(Sha256Test server_test)
add_test(DbConfigParserTest server_test)
add_test(MessageServiceTest server_test)
add_test(AuthServiceTest server_test)
add_test(HttpHandlerTest server_test)