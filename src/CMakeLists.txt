set(SRCS
	timer.cpp
	util.cpp
	http_data.cpp
	uepoll.cpp
	channel.cpp
	event_loop.cpp

	)



include_directories(${PROJECT_SOURCE_DIR}/src/base)

add_executable(tiny_WebServer ${SRCS})
target_link_libraries(tiny_WebServer lib_base)

add_subdirectory(base)