set(SRCS
	timer.cpp
	util.cpp
	http_data.cpp
	uepoll.cpp
	channel.cpp
	event_loop.cpp
	event_loop_thread.cpp
	event_loop_thread_pool.cpp
	server.cpp
	main.cpp

	)



include_directories(${PROJECT_SOURCE_DIR}/src/base)

add_executable(tiny_WebServer ${SRCS})
target_link_libraries(tiny_WebServer base)

add_subdirectory(base)