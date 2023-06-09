


#include <assert.h>

#include "thread.h"


uthread::uthread(const uthread::thread_func_type& func, const string &name) :
	started_(false),
	joined_(false),
	m_thread(),
	m_thread_func(func),
	m_thread_name(name),
	m_latch(1)
{
	set_default_name();
}

uthread::~uthread() {
	if(started_ && !joined_ && m_thread.joinable()) m_thread.detach();
}

void uthread::start() {
	assert(!started_);
	started_ = true;
	thread t(std::bind(&uthread::start_thread, this), "async_logging");
	m_latch.wait();
	t.swap(m_thread);
	if(t.joinable()) t.detach();
}

void uthread::join() {
	assert(started_);
	assert(!joined_);
	joined_ = true;
	if(m_thread.joinable()) {
		m_thread.join();
	}
	else {
		joined_ = false;
		throw "uthread::join(): m_thread is unjoinable";
	}
}

void uthread::set_default_name() {
	if(m_thread_name.empty()) {
		char buf[32];
		snprintf(buf, sizeof buf, "Thread");
		m_thread_name = buf;
	}
}

// 确保thread_func启动之后再count_down
void uthread::start_thread() {
	m_thread_func();
	m_latch.count_down();
	m_thread_name = "finished";
}


