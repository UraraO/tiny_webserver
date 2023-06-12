
/*
 * timer,定时器类，用于多种计时和超时处理
 * timer_node是一个计时器节点，其中保存一个超时时间点，代表未来的某一时间点，超过该点后即超时
 *
 * */


#include <unistd.h>
//#include <deque>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <chrono>

//#include "http_data.h"

using namespace std::chrono;

//using std::deque;
using std::vector;
using std::shared_ptr;
using std::priority_queue;
using std::mutex;
using std::lock_guard;
using std::unique_lock;


class http_data;

class timer_node{
public:
	using http_data_sp = shared_ptr<http_data>;
	// timeout是一个毫秒数，代表从现在起，timeout毫秒后的时间点超时，用于wait_until类似的函数
	timer_node(http_data_sp data, size_t timeout);
	~timer_node();
	timer_node(const timer_node &other);

	void update(size_t timeout);
	bool is_valid();
	void clear_req();
	void set_deleted() { deleted_ = true; }
	bool is_deleted() const { return deleted_; }
	auto get_timeout() const -> decltype(system_clock::now()) { return expire_time_; }

private:
	bool deleted_;
	decltype(system_clock::now()) expire_time_;
	http_data_sp m_data_sp;
};

// 函数对象，用于自定义timer_manager类中的优先队列比较方法，使得最先过期的timer_node在优先队列的top()，便于访问最先过期的节点
class timer_node_cmp{
public:
	bool operator() (const shared_ptr<timer_node> &lhs, const shared_ptr<timer_node> &rhs) {
		return lhs->get_timeout() > rhs->get_timeout();
	}
};

class timer_manager{
public:
	using node_sp = shared_ptr<timer_node>;
	using http_data_sp = shared_ptr<http_data>;

	timer_manager() = default;
	~timer_manager() = default;

	void add_timer(http_data_sp data, size_t timeout);
	void handle_expire_event();

private:
	priority_queue<node_sp , vector<node_sp>, timer_node_cmp> node_queue;
};




















