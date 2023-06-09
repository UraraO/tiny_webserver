
/*
 * ulog类是日志的接口类
 * 通过ulog对象来进行日志记录
 * 最终实现通过ulog设定日志文件名，然后直接向LOG << "log_line”; 即可记录日志
 *
 * */

#pragma once

#include <string>

#include "log_stream.h"
#include "async_log.h"

using std::string;

class ulog{
public:
	ulog(const char *log_filename, int line);
	ulog(string log_filename, int line);
	~ulog();

	log_stream& stream() { return m_stream; }

	static void set_log_filename(string filename) {
		m_log_file_name = filename;
	}
	static string get_log_filename() {
		return m_log_file_name;
	}

	void format_time();

private:
	log_stream m_stream;
	static string m_log_file_name;  // 日志文件webserver.log
	string base_name_;              // 使用日志系统的文件
	int line_;  // 指记录日志的行数
};

string current_time();

//constexpr log_stream& LOG = ulog(ulog::get_log_filename(), __LINE__).stream();
#define LOG (ulog(__FILE__, __LINE__).stream())
















