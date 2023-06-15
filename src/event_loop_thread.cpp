


#include "event_loop_thread.h"


event_loop_thread::event_loop_thread() :
	loop_(nullptr),
	exited_(false),
	thread_([this] { thread_func(); })
{}

event_loop_thread::~event_loop_thread() {
	exited_ = true;
	if (loop_ != nullptr) {
		loop_->quit();
		thread_.join();
	}
}

event_loop *event_loop_thread::start_loop() {
	assert(!thread_.is_start());
	thread_.start();
	{
		unique_lock lock(m_mut);
		while (loop_ == nullptr) m_cond.wait(lock, [this]{ return loop_ != nullptr; });
	}
	return loop_;
}

void event_loop_thread::thread_func() {
	event_loop loop;

	{
		lock_guard lock(m_mut);
		loop_ = &loop;
		m_cond.notify_one();
	}

	loop.loop();
	// assert(exiting_);
	loop_ = nullptr;
}
