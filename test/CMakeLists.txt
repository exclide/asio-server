CMAKE_MINIMUM_REQUIRED(VERSION 3.26)

project(server_test)

find_package(GTest CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

set(sources main.cpp ../src/Sha256.cpp)
set(libs OpenSSL::SSL GTest::gtest GTest::gtest_main)


add_executable(server_test ${sources})
target_link_libraries(server_test ${libs})

add_test(Sha256Test main)