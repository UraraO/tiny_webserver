


#include "event_loop_thread_pool.h"


event_loop_thread_pool::event_loop_thread_pool(event_loop *base_loop, int thread_num) :
	base_loop_(base_loop),
	started_(false),
	thread_num_(thread_num),
	next_(0)
{
	if (thread_num_ <= 0) {
		LOG << "event_loop_thread_pool: thread_num_ <= 0";
		abort();
	}
}

void event_loop_thread_pool::start() {
	base_loop_->assert_in_loop_thread();
	started_ = true;
	for (int i = 0; i < thread_num_; ++i) {
		shared_ptr<event_loop_thread> t(new event_loop_thread());
		m_threads.push_back(t);
		m_loops.push_back(t->start_loop());
	}
}

event_loop *event_loop_thread_pool::get_next_loop() {
	base_loop_->assert_in_loop_thread();
	assert(started_);
	event_loop *loop = base_loop_;
	if (!m_loops.empty()) {
		loop = m_loops[next_];
		next_ = (next_ + 1) % thread_num_;
	}
	return loop;
}
