/*
 * 线程同步机制封装类
 *
 * 内容包含：
 *      互斥锁类
 *      条件变量类
 *      信号量类
 *
 */

#ifndef SYNC_LOCKER_H
#define SYNC_LOCKER_H

#include <pthread.h>
#include <exception>
#include <semaphore.h>

using std::exception;

// 互斥锁类
class locker {
public:
	locker() {
		if(pthread_mutex_init(&ins_mutex, nullptr) != 0) {
			throw exception();
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
class cond {
public:
	cond() {
		if(pthread_cond_init(&ins_cond, nullptr) != 0) {
			throw exception();
		}
	}

	~cond() {
		pthread_cond_destroy(&ins_cond);
	}

	bool wait(pthread_mutex_t * mutex) {
		return pthread_cond_wait(&ins_cond, mutex) == 0;
	}

	bool timedwait(pthread_mutex_t * mutex, struct timespec time) {
		return pthread_cond_timedwait(&ins_cond, mutex, &time) == 0;
	}

	bool signal() {
		return pthread_cond_signal(&ins_cond) == 0;
	}

	bool broadcast() {
		return pthread_cond_broadcast(&ins_cond) == 0;
	}

private:
	pthread_cond_t ins_cond;
};

class sem {
public:
	sem() {
		if(sem_init(&ins_sem, 0, 0) != 0) {
			throw exception();
		}
	}

	sem(const int num) {
		if(sem_init(&ins_sem, 0, num) != 0) {
			throw exception();
		}
	}

	~sem() {
		sem_destroy(&ins_sem);
	}

	bool wait() {
		return sem_wait(&ins_sem) == 0;
	}

	bool post() {
		return sem_post(&ins_sem) == 0;
	}
private:
	sem_t ins_sem;
};

#endif