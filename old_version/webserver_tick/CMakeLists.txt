cmake_minimum_required(VERSION 3.24)
project(webserver_tick)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(webserver_tick
	http_conn.cpp
	http_conn.h
	locker.h
	log.cpp
	log.h
	lst_timer.cpp
	lst_timer.h
	main.cpp
	threadpool.h)
