
/*
 * channel包装一个文件描述符fd
 * 封装了文件描述符对应的处理方法，读，写，连接
 * 并保存与其关联的事件，及fd的所有者
 *
 * */


#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

//#include "timer.h"
#include "http_data.h"


using std::function;
using std::shared_ptr;
using std::weak_ptr;
using std::string;
using std::unordered_map;


class event_loop;
//class http_data;



class channel{
public:
	using channel_sp = shared_ptr<channel>;
	using callback = function<void()>;

	explicit channel(event_loop *loop);
	channel(event_loop *loop, int fd);
	~channel();
	int get_fd() const { return fd_; }
	void set_fd(int fd) { fd_ = fd; }

	void set_holder(shared_ptr<http_data> holder) { holder_ = holder; }
	shared_ptr<http_data> get_holder() {
		shared_ptr<http_data> ret(holder_.lock());
		return ret;
	}

	void set_read_handler(callback &&read_handler) { read_handler_ = read_handler; }
	void set_write_handler(callback &&write_handler) {
		write_handler_ = write_handler;
	}
	void set_error_handler(callback &&error_handler) {
		error_handler_ = error_handler;
	}
	void set_conn_handler(callback &&conn_handler) { conn_handler_ = conn_handler; }

	void handle_events() {
		events_ = 0;
		if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
			events_ = 0;
			return;
		}
		if (revents_ & EPOLLERR) {
			if (error_handler_) error_handler_();
			events_ = 0;
			return;
		}
		if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
			handle_read();
		}
		if (revents_ & EPOLLOUT) {
			handle_write();
		}
		handle_conn();
	}
	void handle_read();
	void handle_write();
	//void handle_error(int fd, int err_num, std::string short_msg);
	void handle_error();
	void handle_conn();

	void set_revents(__uint32_t ev) { revents_ = ev; }

	void set_events(__uint32_t ev) { events_ = ev; }
	__uint32_t &get_events() { return events_; }

	bool equal_update_last_events() {
		bool ret = (last_events_ == events_);
		last_events_ = events_;
		return ret;
	}

	__uint32_t get_last_events() const { return last_events_; }

private:
	event_loop *loop_;
	int fd_;
	__uint32_t events_;
	__uint32_t revents_;
	__uint32_t last_events_;

	weak_ptr<http_data> holder_;

	callback read_handler_;
	callback write_handler_;
	callback conn_handler_;
	callback error_handler_;
};

















