

#include <cassert>
#include <unistd.h>
#include <chrono>


#include "log_file.h"
#include "async_log.h"

using namespace std::chrono;


async_log::async_log(const string &basename, int interval) :
	flush_interval_(interval),
	m_file_name(basename),
	running_(false),
	m_thread([this] { thread_func(); }, "async_logging"),
	m_mut(),
	m_cond(),
	current_buffer_ptr_(new buffer),
	next_buffer_ptr_(new buffer),
	m_buffers(),
	m_latch(1)
{
	assert(basename.size() > 1);
	current_buffer_ptr_->reset_data();
	next_buffer_ptr_->reset_data();
}

void async_log::append(const char *logline, size_t length) {
	lock_guard lock(m_mut);
	if(current_buffer_ptr_->available_size() > length) {
		current_buffer_ptr_->append(logline, length);
	}
	else {
		m_buffers.push_back(current_buffer_ptr_);
		current_buffer_ptr_.reset();
		if (next_buffer_ptr_)
			current_buffer_ptr_ = std::move(next_buffer_ptr_);
		else
			current_buffer_ptr_.reset(new buffer);
		current_buffer_ptr_->append(logline, length);
		m_cond.notify_one();
	}
}

void async_log::thread_func() {
	assert(running_);
	m_latch.count_down();
	log_file output(m_file_name);
	buffer_ptr buffer1(new buffer);
	buffer_ptr buffer2(new buffer);
	buffer1->reset_data();
	buffer2->reset_data();
	buffer_vec buffers_need_write;
	buffers_need_write.reserve(16);
	while (running_) {
		// 每次循环开始都保证临时缓冲区buffer1，buffer2和双缓冲区其一的buffers_need_write为空
		assert(buffer1 && buffer1->length() == 0);
		assert(buffer2 && buffer2->length() == 0);
		assert(buffers_need_write.empty());
		// 此处交换双缓冲区buffers_need_write和m_buffers
		{
			unique_lock lock(m_mut);
			if(m_buffers.empty())  // 不常用的类内缓冲区
			{
				m_cond.wait_for(lock, seconds(flush_interval_));
			}
			m_buffers.push_back(current_buffer_ptr_);
			current_buffer_ptr_.reset();

			current_buffer_ptr_ = std::move(buffer1);
			buffers_need_write.swap(m_buffers);
			if (!next_buffer_ptr_) {
				next_buffer_ptr_ = std::move(buffer2);
			}
		}

		assert(!buffers_need_write.empty());

		if (buffers_need_write.size() > 25) {
			buffers_need_write.erase(buffers_need_write.begin() + 2, buffers_need_write.end());
		}

		for (auto & buffer : buffers_need_write) {
			output.append_unlocked(buffer->data(), buffer->length());
		}

		if (buffers_need_write.size() > 2) {
			buffers_need_write.resize(2);
		}

		if (!buffer1) {
			assert(!buffers_need_write.empty());
			buffer1 = buffers_need_write.back();
			buffers_need_write.pop_back();
			buffer1->reset_cur();
		}

		if (!buffer2) {
			assert(!buffers_need_write.empty());
			buffer2 = buffers_need_write.back();
			buffers_need_write.pop_back();
			buffer2->reset_cur();
		}

		buffers_need_write.clear();
		output.flush();
	}
	output.flush();
}












