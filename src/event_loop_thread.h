

/*
 * event_loop_thread简单封装了event_loop，一个线程中仅运行一个event_loop
 *
 * */


#pragma once

#include <condition_variable>
#include <mutex>

#include "event_loop.h"
#include "uthread.h"


using std::condition_variable;
using std::mutex;
using std::lock_guard;
using std::unique_lock;


class event_loop_thread {
public:
	// prevent copy
	event_loop_thread(const event_loop_thread &) = delete;
	event_loop_thread(event_loop_thread &) = delete;
	event_loop_thread& operator= (const event_loop_thread &) = delete;
	event_loop_thread& operator= (event_loop_thread &) = delete;

	event_loop_thread();
	~event_loop_thread();

	event_loop* start_loop();

private:
	void thread_func();
	event_loop* loop_;
	bool exited_;
	uthread thread_;
	mutex m_mut;
	condition_variable m_cond;
};





















