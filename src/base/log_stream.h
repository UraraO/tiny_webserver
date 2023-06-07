
/*
 * log_stream用于格式化输出
 * 封装了大量的 << 运算符，使得各种不同的输入类型，都可以正常的输入进日志流中
 * 例如，LOG << "some log line",log_stream封装了对字符串的 << 操作，使得其可以方便地输入进LOG中
 * log_stream有一个属于自己的缓冲区，缓冲区比较小，用于保存一行日志
 *
 * */

#pragma once

#include <cassert>
#include <cstring>
#include <string>

using std::string;

// fixed_buffer，一个简易的缓冲区，供log_stream缓存一行日志使用，logline
constexpr size_t small_buffer_size = 4000;
constexpr size_t large_buffer_size = 4000 * 1000;

template <size_t BUFFER_SIZE>
class fixed_buffer {
public:
	// prevent copy
	fixed_buffer(const fixed_buffer &) = delete;
	fixed_buffer& operator=(const fixed_buffer &) = delete;

	fixed_buffer() : m_cur(m_data) {}
	~fixed_buffer() = default;

	void append(const char *buf, size_t length) {
		if(available_size() > static_cast<long>(length)) {
			memcpy(m_cur, buf, length);
			m_cur += length;
		}
	}

	const char* data() const { return m_data; }
	long length() const { return static_cast<long>(m_cur - m_data); }

	char* current() { return m_cur; }
	long available_size() const { return static_cast<long>(end() - m_cur); }
	void add_cur(long len) { m_cur += len; }

	void reset_cur() { m_cur = m_data; }
	void reset_data() { memset(m_data, 0, sizeof m_data); }

private:
	const char* end() const { return m_data + sizeof(m_data); }

	char m_data[BUFFER_SIZE]{};
	char *m_cur;

};

class log_stream {
public:
	using buffer_t = fixed_buffer<small_buffer_size>;

	// prevent copy
	log_stream(const log_stream &) = delete;
	log_stream& operator=(const log_stream &) = delete;

	// operator << for many types
	log_stream& operator<<(bool v) {
		m_buffer.append(v ? "1" : "0", 1);
		return *this;
	}

	log_stream& operator<<(char v) {
		m_buffer.append(&v, 1);
		return *this;
	}

	log_stream& operator<<(short);
	log_stream& operator<<(unsigned short);
	log_stream& operator<<(int);
	log_stream& operator<<(unsigned int);
	log_stream& operator<<(long);
	log_stream& operator<<(unsigned long);
	log_stream& operator<<(long long);
	log_stream& operator<<(unsigned long long);

	log_stream& operator<<(float v) {
		*this << static_cast<double>(v);
		return *this;
	}
	log_stream& operator<<(double);
	log_stream& operator<<(long double);

	log_stream& operator<<(const char* str) {
		if (str)
			m_buffer.append(str, strlen(str));
		else
			m_buffer.append("(null)", 6);
		return *this;
	}

	log_stream& operator<<(const unsigned char* str) {
		return operator<<(reinterpret_cast<const char*>(str));
	}

	log_stream& operator<<(const string &v) {
		m_buffer.append(v.c_str(), v.size());
		return *this;
	}


	const buffer_t& buffer() const { return m_buffer; }
	void reset_buffer() { m_buffer.reset_cur(); }

private:
	//void static_check();

	buffer_t m_buffer;

	template <typename T>
	void format_int(T);
	// 若m_buffer容不下max_remain_size个字符的话，则丢弃这些字符
	static const size_t max_remain_size = 32;

};















