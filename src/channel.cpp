


#include "channel.h"

//#include "event_loop.h"



channel::channel(event_loop *loop) :
	loop_(loop),
	fd_(0),
	events_(0),
	last_events_(0)
{}

channel::channel(event_loop *loop, int fd) :
	loop_(loop),
	fd_(fd),
	events_(0),
	last_events_(0)
{}

channel::~channel() {
	// TODO
	// loop_->poller_->epoll_del(fd, events_);
	// close(fd_);
}

void channel::handle_read() {
	if(read_handler_) read_handler_();
}

void channel::handle_write() {
	if(write_handler_) write_handler_();
}

void channel::handle_conn() {
	if(conn_handler_) conn_handler_();
}

void channel::handle_error() {
	if(error_handler_) error_handler_();
}

