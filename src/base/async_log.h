
/*
 * async_log类是日志的核心类
 * 利用下层的log_file实现了异步日志记录
 * async_log有一个专属的线程，使用双缓冲区，实现前端向后端写/后端向磁盘写的分离
 * 提高日志系统的并发度
 *
 * */

#pragma once


#include <functional>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "countdown_latch.h"
#include "log_stream.h"
#include "thread.h"

using std::vector;
using std::string;
using std::mutex;
using std::lock_guard;
using std::unique_lock;
using std::condition_variable;
using std::function;
using std::shared_ptr;

class async_log{
public:
	using buffer = fixed_buffer<large_buffer_size>;
	using buffer_ptr = shared_ptr<buffer>;
	using buffer_vec = vector<buffer_ptr>;

	// prevent copy
	async_log(const async_log&) = delete;
	async_log(async_log&) = delete;
	async_log& operator= (const async_log&) = delete;
	async_log& operator= (async_log&) = delete;

	explicit async_log(const string& basename, int interval = 2);
	~async_log() {
		if(running_) stop();
	}

	void append(const char *logline, size_t length);

	void start() {
		running_ = true;
		m_thread.start();
		m_latch.wait();
	}

	void stop() {
		running_ = false;
		m_cond.notify_one();
		m_thread.join();
	}

private:
	void thread_func();

	const unsigned flush_interval_;
	bool running_;
	string m_file_name;
	uthread m_thread;
	mutex m_mut;
	condition_variable m_cond;
	countdown_latch m_latch;
	buffer_ptr current_buffer_ptr_;
	buffer_ptr next_buffer_ptr_;
	buffer_vec m_buffers;
};










