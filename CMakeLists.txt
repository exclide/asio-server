CMAKE_MINIMUM_REQUIRED(VERSION 3.26)

project(server)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)