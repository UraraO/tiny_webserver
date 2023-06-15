
/*
 * event_loop是webserver的核心部分
 * event_loop
 *
 * */



#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <mutex>
//#include "channel.h"
#include "uepoll.h"
#include "util.h"
#include "ulog.h"
#include "uthread.h"


using std::function;
using std::shared_ptr;
using std::vector;
using std::mutex;
using std::lock_guard;
using std::unique_lock;


//class channel;

class event_loop {
public:
	using functor = function<void()>;

	event_loop();
	~event_loop();
	void loop();
	void quit();
	void run_in_loop(functor &&func);
	void queue_in_loop(functor &&func);
	bool is_in_loop_thread() const { return thread_id_ == this_thread::get_id(); }
	void assert_in_loop_thread() { assert(is_in_loop_thread()); }
	static void shutdown(shared_ptr<channel> channel) { shutDownWR(channel->get_fd()); }
	void remove_from_poller(shared_ptr<channel> channel) {
		// shutDownWR(channel->getFd());
		poller_->uepoll_del(channel);
	}
	void update_poller(shared_ptr<channel> channel, int timeout = 0) {
		poller_->uepoll_mod(channel, timeout);
	}
	void add_to_poller(shared_ptr<channel> channel, int timeout = 0) {
		poller_->uepoll_add(channel, timeout);
	}

private:
	bool looping_;
	shared_ptr<uepoll> poller_;
	int wakeup_fd_;
	bool quited_;
	bool event_handling_;
	mutable mutex m_mut;
	vector<functor> m_pending_functors;
	bool calling_pending_functors_;
	thread::id thread_id_;
	shared_ptr<channel> m_wakeup_channel_sp;

	void wakeup() const;
	void handleRead();
	void doPendingFunctors();
	void handleConn();
};



















