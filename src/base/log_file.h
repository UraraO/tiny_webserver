
/*
 * log_file封装了file_util中的append_file类
 * append_file是底层用于直接写日志文件的类
 * log_file包装了无锁和加锁append方法
 * 提供了一个计数器，可以规定 N 次append后再刷新缓冲区
 *
 * */

/*
 * test success
 * */

#pragma once

#include <memory>
#include <string>
#include <mutex>

#include "file_util.h"

using std::string;
using std::unique_ptr;
using std::mutex;
using std::lock_guard;

class log_file {
public:
	// prevent copy
	log_file(const log_file &other) = delete;
	log_file(log_file &other) = delete;
	log_file& operator=(const log_file &other) = delete;
	log_file& operator=(log_file &other) = delete;

	~log_file() = default;

	explicit log_file(const string &basename, int N = 1024);

	void append_unlocked(const char *logline, size_t length);
	//void append(const char *logline, size_t length);
	void flush();
	// bool roll_file();

private:
	const string m_basename;
	const int flush_per_N;
	int m_count;

	mutex m_mutex;
	unique_ptr<append_file> m_up_append_file;
};















