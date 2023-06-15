

#include <cassert>
#include <queue>

#include "uepoll.h"
#include "util.h"
#include "ulog.h"


using std::queue;

constexpr int EVENTSNUM = 4096;
constexpr int EPOLLWAIT_TIME = 10000;


uepoll::uepoll() :
	epoll_fd_(epoll_create1(EPOLL_CLOEXEC)),
	m_events(EVENTSNUM)
{
	assert(epoll_fd_ > 0);
}

void uepoll::uepoll_add(uepoll::channel_sp request, int timeout) {
	int fd = request->get_fd();
	if(timeout > 0) {
		add_timer(request, timeout);
		fd_http_[fd] = request->get_holder();
	}

	epoll_event event;
	event.data.fd = fd;
	event.events = request->get_events();

	//request->EqualAndUpdateLastEvents();
	request->equal_update_last_events();

	fd_channel_[fd] = request;
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) < 0) {
		perror("epoll_add error");
		fd_channel_[fd].reset();
	}
}

void uepoll::uepoll_mod(uepoll::channel_sp request, int timeout) {
	if (timeout > 0) add_timer(request, timeout);
	int fd = request->get_fd();
	if (!request->equal_update_last_events()) {
		epoll_event event;
		event.data.fd = fd;
		event.events = request->get_events();
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event) < 0) {
			perror("uepoll:uepoll_mod error");
			fd_channel_[fd].reset();
		}
	}
}

void uepoll::uepoll_del(uepoll::channel_sp request) {
	int fd = request->get_fd();
	epoll_event event;
	event.data.fd = fd;
	event.events = request->get_last_events();
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event) < 0) {
		perror("uepoll::uepoll_del error");
	}
	fd_channel_[fd].reset();
	fd_http_[fd].reset();
	fd_channel_.erase(fd);
	fd_http_.erase(fd);
}

vector<shared_ptr<channel>> uepoll::poll() {
	while (true) {
		int event_count = epoll_wait(epoll_fd_, &*m_events.begin(), m_events.size(), EPOLLWAIT_TIME);
		if (event_count < 0) perror("epoll wait error");
		vector<channel_sp> req_data = get_events_request(event_count);
		if (!req_data.empty()) return req_data;
	}
}

vector<uepoll::channel_sp> uepoll::get_events_request(int events_num) {
	vector<channel_sp> req_data;
	for (int i = 0; i < events_num; ++i) {
		// 获取有事件产生的描述符
		int fd = m_events[i].data.fd;

		channel_sp cur_req = fd_channel_[fd];

		if (cur_req) {
			cur_req->set_revents(m_events[i].events);
			cur_req->set_events(0);
			// 加入线程池之前将Timer和request分离
			// cur_req->seperateTimer();
			req_data.push_back(cur_req);
		} else {
			LOG << "uepoll::get_events_request, cur_req_sp is invalid";
		}
	}
	return req_data;
}

void uepoll::add_timer(uepoll::channel_sp request_data, int timeout) {
	auto holder = request_data->get_holder();
	if(holder) m_timer_manager.add_timer(holder, timeout);
	else LOG << "uepoll::add_timer fail, request's holder has destroyed";
}

void uepoll::handle_expired() {
	m_timer_manager.handle_expire_event();
}
