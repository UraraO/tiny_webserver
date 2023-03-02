/*
 * 线程同步机制封装类
 *
 * 内容包含：
 *      互斥锁类
 *      条件变量类
 *
 */

#ifndef SYNC_LOCKER_H
#define SYNC_LOCKER_H

#include <pthread.h>
#include <exception>

using std::exception;

// 互斥锁类
class locker {
public:
	locker() {
		if(pthread_mutex_init(&ins_mutex, nullptr) != 0) {
			throw exception("locker() mutex init");
		}
	}

	~locker() {
		pthread_mutex_destroy(&ins_mutex);
	}

	bool lock() {
		return pthread_mutex_lock(&ins_mutex) == 0;
	}

	bool unlock() {
		return pthread_mutex_unlock(&ins_mutex) == 0;
	}

	pthread_mutex_t * get() {
		return &ins_mutex;
	}
private:
	pthread_mutex_t ins_mutex;
};

// 条件变量类

#endif