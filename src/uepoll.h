


/*
 * uepoll将epoll封装为poller类
 *
 * */

#pragma once

#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>

#include "channel.h"
#include "http_data.h"
//#include "timer.h"


using std::shared_ptr;
using std::unordered_map;
using std::vector;


class timer_node;
class timer_manager;

class uepoll {
public:
	using channel_sp = shared_ptr<channel>;

	uepoll();
	~uepoll() = default;

	void uepoll_add(channel_sp request, int timeout);
	void uepoll_mod(channel_sp request, int timeout);
	void uepoll_del(channel_sp request);

	vector<shared_ptr<channel>> poll();
	vector<shared_ptr<channel>> get_events_request(int events_num);
	void add_timer(shared_ptr<channel> request_data, int timeout);
	int get_epoll_fd() { return epoll_fd_; }
	void handle_expired();

private:
	static constexpr int MAXFD = 100000;
	int epoll_fd_;
	vector<epoll_event> m_events;
	//shared_ptr<channel> fd_channel_[MAXFD];
	//shared_ptr<http_data> fd_http_[MAXFD];
	unordered_map<int, shared_ptr<channel>> fd_channel_;
	unordered_map<int, shared_ptr<http_data>> fd_http_;
	timer_manager m_timer_manager;
};




















