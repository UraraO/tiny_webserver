
/*
 * thread对std::thread做出一层封装
 * 在my_thread内部保存thread_func，作为线程的工作函数
 * 对外仅留出start()和join()接口，用于启动和合并线程
 *
 * */

#pragma once
#include <syscall.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "countdown_latch.h"

using std::function;
using std::string;
using std::thread;


class uthread {
public:
	using thread_func_type = function<void()>;

	// prevent copy
	uthread(const uthread&) = delete;
	uthread(uthread&) = delete;
	uthread& operator= (const uthread&) = delete;
	uthread& operator= (uthread&) = delete;

	explicit uthread(const thread_func_type &, const string& name = string());
	~uthread();

	void start();
	void join();
	bool is_start() const { return started_; }
	thread::id get_id() { return m_thread.get_id(); }
	const string& get_name() const { return m_thread_name; }

private:
	void set_default_name();
	void start_thread();
	bool started_;
	bool joined_;
	thread m_thread;
	thread_func_type m_thread_func;
	string m_thread_name;
	countdown_latch m_latch;
};




















