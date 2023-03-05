add_subdirectory(thread_sync)

include_directories(include)

#add_custom_target(thread_pool thread_pool.cpp thread_sync/locker.cpp)