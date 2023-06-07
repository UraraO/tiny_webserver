



#include "log_file.h"

log_file::log_file(const string basename, int N = 1024) :
	m_basename(basename),
	flush_per_N(N),
	m_count(0),
	m_up_mutex(new mutex),
	m_up_append_file(std::make_unique<append_file>(basename))
{}

// lock-free append
void log_file::append_unlocked(const char *logline, size_t length) {
	m_up_append_file->append(logline, length);
	++m_count;
	if(m_count >= flush_per_N) {
		m_up_append_file->flush();
		m_count = 0;
	}
}

// with-lock append
void log_file::append(const char *logline, size_t length) {
	lock_guard guard(*m_up_mutex);
	append_unlocked(logline, length);
}

void log_file::flush() {
	lock_guard guard(*m_up_mutex);
	m_up_append_file->flush();
}




















