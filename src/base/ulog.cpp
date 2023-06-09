

#include <chrono>
#include <mutex>
#include <memory>
#include <sstream>
#include <iomanip>
#include <utility>
#include <unistd.h>

#include "ulog.h"
#include "async_log.h"

using namespace std::chrono;
using std::call_once;
using std::shared_ptr;
using std::stringstream;


static shared_ptr<async_log> alog;
string ulog::m_log_file_name = "./webserver.log";
std::once_flag once_flag_;

void ulog_init_once() {
	alog = std::make_shared<async_log>(ulog::get_log_filename());
	alog->start();
}

void logging(const char *msg, size_t length) {
	call_once(once_flag_, ulog_init_once);
	alog->append(msg, length);
}

ulog::ulog(const char *log_filename, int line) :
	base_name_(log_filename),
	line_(line),
	m_stream()
{
	format_time();
}

ulog::ulog(string log_filename, int line) :
	base_name_(std::move(log_filename)),
	line_(line),
	m_stream()
{
	format_time();
}

ulog::~ulog() {
	m_stream << " -- " << base_name_ << ':' << line_ << '\n';
	const log_stream::buffer_t &buf(stream().buffer());
	logging(buf.data(), buf.length());
}

void ulog::format_time() {
	m_stream << current_time();
}

// 日期格式化
static string time_format_ = "%Y/%m/%d  %H:%M:%S";
string current_time()
{
	stringstream ss;
	auto now = system_clock::now();
	time_t t1 = system_clock::to_time_t(now);
	ss << std::put_time(localtime(&t1), time_format_.c_str());
	string time = ss.str();
	time += "  ";
	return time;
}













