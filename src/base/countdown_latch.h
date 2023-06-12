

/*
 * countdownlatch是一种同步工具，用于等待多个并发事件的完成，相当于一种轻量级的future
 * 其内部含有一个计数器，一个阻塞队列，当计数器减少至0时，阻塞队列中的等待线程唤醒，开始执行线程任务
 * 本程序中的countdownlatch用于确保thread的func真正启动后，才让thread.start返回
 *
 * */
#pragma once

#include <mutex>
#include <condition_variable>

using std::mutex;
using std::condition_variable;


class countdown_latch {
public:
	// 禁止拷贝
	// prevent copy
	countdown_latch(const countdown_latch &other) = delete;
	countdown_latch(countdown_latch &other) = delete;
	countdown_latch &operator=(const countdown_latch &other) = delete;
	countdown_latch &operator=(countdown_latch &other) = delete;

	explicit countdown_latch(int count) : m_count(count) {}

	void wait();
	void count_down();

private:
	mutable mutex m_mut;
	condition_variable m_cond;
	int m_count;
};



#include <iostream>
using namespace std;





















