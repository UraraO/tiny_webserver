set(BASE_SRCS
	countdown_latch.cpp
	file_util.cpp
	log_file.cpp
	async_log.cpp
	log_stream.cpp
	uthread.cpp
	ulog.cpp

	)

add_library(base ${BASE_SRCS})

#add_executable(async_log async_log.cpp)
#add_executable(uthread uthread.cpp)
#add_executable(ulog ulog.cpp)
#add_executable(file_util file_util.cpp)
#add_executable(log_file log_file.cpp)

#target_link_libraries(async_log base)
#target_link_libraries(uthread base)
#target_link_libraries(ulog base)
#target_link_libraries(file_util base)
#target_link_libraries(log_file base)


