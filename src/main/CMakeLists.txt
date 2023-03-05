include_directories(include)

add_executable(server main.cpp)

target_link_libraries(server http_connection locker)