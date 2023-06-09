
/*
 * 最底层的文件类，其中维护打开的日志文件，析构的时候自动fclose日志文件
 * append函数直接写日志文件
 * flush函数刷新文件缓冲区
 *
 * */

/*
 * test success
 * */

#pragma once

#include <string>

using std::string;

constexpr size_t BUFFER_SIZE = 64 * 1024;

class append_file {
public:
	// 禁止拷贝
	// prevent copy
	append_file(const append_file &other) = delete;
	append_file(append_file &other) = delete;
	append_file& operator=(const append_file &other) = delete;
	append_file& operator=(append_file &other) = delete;

	explicit append_file(const string& filename);
	~append_file();

	// append(),向磁盘文件写入
	void append(const char *logline, const size_t length);
	// flush(),刷新缓冲区
	void flush();

private:
	size_t write(const char *logline, const size_t length);
	FILE *m_fp;
	char m_buffer[BUFFER_SIZE]{};
};




















