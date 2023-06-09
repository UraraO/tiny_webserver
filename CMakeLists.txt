cmake_minimum_required(VERSION 3.24)
project(tiny_WebServer CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_subdirectory(src)

message("1. PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
message("2. PROJECT_SOURCE _DIR = ${PROJECT_SOURCE_DIR}")

# add pthread lib
#find_package(Threads REQUIRED)
#target_link_libraries(tiny_Webserver PUBLIC Threads::Threads)
#add_executable(tiny_WebServer main.cpp)
