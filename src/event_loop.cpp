

#include <cstdlib>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "util.h"
#include "ulog.h"
#include "event_loop.h"


__thread event_loop *t_loop_in_this_thread = nullptr;


int create_event_fd() {
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0) {
		LOG << "event_loop.cpp: create_event_fd(): Failed in eventfd";
		abort();
	}
	return evtfd;
}

event_loop::event_loop() :
	looping_(false),
	poller_(new uepoll()),
	wakeup_fd_(create_event_fd()),
	quited_(false),
	event_handling_(false),
	calling_pending_functors_(false),
	thread_id_(this_thread::get_id()),
	m_wakeup_channel_sp(new channel(this, wakeup_fd_))
{
	if(t_loop_in_this_thread) {}
	else {
		t_loop_in_this_thread = this;
	}
	// m_wakeup_channel_sp->set_events(EPOLLIN | EPOLLET | EPOLLONESHOT);
	m_wakeup_channel_sp->set_events(EPOLLIN | EPOLLET);
	m_wakeup_channel_sp->set_read_handler(bind(&event_loop::handleRead, this));
	m_wakeup_channel_sp->set_conn_handler(bind(&event_loop::handleConn, this));
	poller_->uepoll_add(m_wakeup_channel_sp, 0);
}

event_loop::~event_loop() {
	close(wakeup_fd_);
	t_loop_in_this_thread = nullptr;
}

void event_loop::loop() {
	assert(!looping_);
	assert(is_in_loop_thread());
	looping_ = true;
	quited_ = false;
	// LOG_TRACE << "EventLoop " << this << " start looping";
	vector<shared_ptr<channel>> ret;
	while (!quited_) {
		// cout << "doing" << endl;
		ret.clear();
		ret = std::move(poller_->poll());
		event_handling_ = true;
		for (auto &it : ret) it->handle_events();
		event_handling_ = false;
		doPendingFunctors();
		poller_->handle_expired();
	}
	looping_ = false;
}

void event_loop::quit() {
	quited_ = true;
	if (!is_in_loop_thread()) {
		wakeup();
	}
}

void event_loop::run_in_loop(event_loop::functor &&func) {
	if (is_in_loop_thread()) func();
	else queue_in_loop(std::move(func));
}

void event_loop::queue_in_loop(event_loop::functor &&func) {
	{
		lock_guard lock(m_mut);
		m_pending_functors.emplace_back(std::move(func));
	}
	if (!is_in_loop_thread() || calling_pending_functors_) wakeup();
}

void event_loop::wakeup() const {
	uint64_t one = 1;
	ssize_t n = writen(wakeup_fd_, (char*)(&one), sizeof one);
	if (n != sizeof one) {
		LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
	}
}

void event_loop::handleRead() {
	uint64_t one = 1;
	ssize_t n = readn(wakeup_fd_, &one, sizeof one);
	if (n != sizeof one) {
		LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
	}
	// m_wakeup_channel_sp->set_events(EPOLLIN | EPOLLET | EPOLLONESHOT);
	m_wakeup_channel_sp->set_events(EPOLLIN | EPOLLET);
}

void event_loop::doPendingFunctors() {
	vector<functor> functors;
	calling_pending_functors_ = true;

	{
		lock_guard lock(m_mut);
		functors.swap(m_pending_functors);
	}

	for (const auto &functor : functors)
		functor();
	calling_pending_functors_ = false;
}

void event_loop::handleConn() {
	update_poller(m_wakeup_channel_sp, 0);
}
