


#include <algorithm>
#include "log_stream.h"



const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

// From muduo
template <typename T>
size_t convert(char buf[], T value) {
	T i = value;
	char* p = buf;

	do {
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);

	if (value < 0) {
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

template <typename T>
void log_stream::format_int(T v) {
	// buffer容不下kMaxNumericSize个字符的话会被直接丢弃
	if (m_buffer.available_size() >= max_remain_size) {
		size_t len = convert(m_buffer.current(), v);
		m_buffer.add_cur(static_cast<long>(len));
	}
}


log_stream &log_stream::operator<<(short v) {
	*this << static_cast<int>(v);
	return *this;
}

log_stream &log_stream::operator<<(unsigned short v) {
	*this << static_cast<unsigned int>(v);
	return *this;
}

log_stream &log_stream::operator<<(int v) {
	format_int(v);
	return *this;
}

log_stream &log_stream::operator<<(unsigned int v) {
	format_int(v);
	return *this;
}

log_stream &log_stream::operator<<(long v) {
	format_int(v);
	return *this;
}

log_stream &log_stream::operator<<(unsigned long v) {
	format_int(v);
	return *this;
}

log_stream &log_stream::operator<<(long long int v) {
	format_int(v);
	return *this;
}

log_stream &log_stream::operator<<(unsigned long long int v) {
	format_int(v);
	return *this;
}

log_stream &log_stream::operator<<(double v) {
	if (m_buffer.available_size() >= max_remain_size) {
		int len = snprintf(m_buffer.current(), max_remain_size, "%.12g", v);
		m_buffer.add_cur(len);
	}
	return *this;
}

log_stream &log_stream::operator<<(long double v) {
	if (m_buffer.available_size() >= max_remain_size) {
		int len = snprintf(m_buffer.current(), max_remain_size, "%.12Lg", v);
		m_buffer.add_cur(len);
	}
	return *this;
}





