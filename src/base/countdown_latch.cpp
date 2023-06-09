

#include "countdown_latch.h"

using std::unique_lock;
using std::lock_guard;

void countdown_latch::wait() {
	unique_lock guard(m_mut);
	while(m_count > 0) m_cond.wait(guard, [this]{ return m_count <= 0; });
}

void countdown_latch::count_down() {
	lock_guard guard(m_mut);
	--m_count;
	if(m_count == 0) m_cond.notify_all();
}





















