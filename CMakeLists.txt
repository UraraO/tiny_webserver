cmake_minimum_required(VERSION 3.24)
project(tiny_WebServer CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_subdirectory(src ../bin)

# add pthread lib
find_package(Threads REQUIRED)
#target_link_libraries(remote_test PUBLIC Threads::Threads)
#add_executable(tiny_WebServer main.cpp)
