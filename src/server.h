

/*
 * server包装了整个webserver的业务逻辑
 *
 * */


#pragma once

#include <memory>

//#include "channel.h"
#include "event_loop.h"
#include "event_loop_thread_pool.h"


using std::unique_ptr;
using std::shared_ptr;


class server {
public:
	server(event_loop *loop, int thread_num, int port);
	~server() = default;
	event_loop *get_loop() const { return loop_; }
	void start();
	void handle_new_conn();
	void handle_this_conn() { loop_->update_poller(accept_channel_); }

private:
	event_loop *loop_;
	int thread_num_;
	unique_ptr<event_loop_thread_pool> m_pool;
	bool started_;
	shared_ptr<channel> accept_channel_;
	int port_;
	int listen_fd_;
	static constexpr int MAXFD = 100000;
};



















