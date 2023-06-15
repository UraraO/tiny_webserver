

/*
 * event_loop_thread_pool内含一个线程池，保存着数个event_loop_thread
 *
 * */


#pragma once


#include <vector>
#include <memory>

#include "ulog.h"
#include "event_loop_thread.h"


using std::vector;
using std::shared_ptr;


class event_loop_thread_pool {
public:
	// prevent copy
	event_loop_thread_pool(const event_loop_thread &) = delete;
	event_loop_thread_pool(event_loop_thread &) = delete;
	event_loop_thread_pool& operator= (const event_loop_thread &) = delete;
	event_loop_thread_pool& operator= (event_loop_thread &) = delete;

	explicit event_loop_thread_pool(event_loop *base_loop, int thread_num = 4);

	~event_loop_thread_pool() { LOG << "event_loop_thread_pool destroyed"; }
	void start();

	event_loop* get_next_loop();

private:
	event_loop* base_loop_;
	bool started_;
	int thread_num_;
	int next_;
	vector<shared_ptr<event_loop_thread>> m_threads;
	vector<event_loop*> m_loops;
};



















