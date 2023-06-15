


//#include "timer.h"
#include "http_data.h"

using std::make_shared;
//using http_data_sp = shared_ptr<http_data>;

timer_node::timer_node(timer_node::http_data_sp data, size_t timeout) : deleted_(false),
	m_data_sp(data)
{
	auto old = system_clock::now();
	auto diff = milliseconds(timeout);
	expire_time_ = old + diff;
}

timer_node::~timer_node() {
	if(m_data_sp) m_data_sp->handle_close();
}

timer_node::timer_node(const timer_node &other) = default;

void timer_node::update(size_t timeout) {
	auto old = system_clock::now();
	auto diff = milliseconds(timeout);
	expire_time_ = old + diff;
}

bool timer_node::is_valid() {
	auto now = system_clock::now();
	if(now < expire_time_) {
		return true;
	}
	set_deleted();
	return false;
}

void timer_node::clear_req() {
	m_data_sp.reset();
	set_deleted();
}

////////////////////////////////
// class timer_manager

void timer_manager::add_timer(timer_manager::http_data_sp data, size_t timeout) {
	node_sp new_node = make_shared<timer_node>(data, timeout);
	node_queue.push(new_node);
	data->link_timer(new_node);
}

void timer_manager::handle_expire_event() {
	while (!node_queue.empty()) {
		node_sp a_new_expire_point = node_queue.top();
		if( a_new_expire_point->is_deleted() ||  !a_new_expire_point->is_valid())
			node_queue.pop();
		else break;
	}
}













