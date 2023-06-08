set(BASE_SRCS
	countdown_latch.cpp
	file_util.cpp
	log_file.cpp
	async_log.cpp
	log_stream.cpp
	thread.cpp

	)

add_library(lib_base ${BASE_SRCS})

add_executable(async_log async_log.cpp)
target_link_libraries(async_log lib_base)


