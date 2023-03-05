/*
 * 线程池
 *      1
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <list>
#include <exception>
#include <cstdio>

#include "../thread_sync/include/locker.h"

using std::list;

template<typename T>
class thread_pool {
public:
	thread_pool(int thread_number = 8, int request_max_num = 10000);
	~thread_pool();
	bool append(T *request);

private:
	static void * child_thread_worker(void * arg);
	void run();

private:
	int thread_count;

	pthread_t * thread_list;

	// 请求队列
	int request_max;

	list<T *> list_work;

	locker lock_worklist;

	// 信号量用于判断是否有任务需要处理
	sem stat_list;

	bool thread_stop;
};

template<typename T>
thread_pool<T>::thread_pool(int thread_number, int request_max_num) :
	thread_count(thread_number),
	request_max(request_max_num),
	thread_stop(false),
	thread_list(nullptr) {
	if(thread_number <= 0 || request_max_num <= 0) {
		throw exception();
	}
	thread_list = new pthread_t[thread_number];
	if(!thread_list) {
		throw exception();
	}

	// 创建num个线程，并设置线程脱离
	for(int i = 0; i < thread_number; ++i) {
		if(pthread_create(thread_list + i, nullptr, child_thread_worker, (void *)this) != 0) {
			delete[] thread_list;
			throw exception();
		}
		printf("making %dth thread\n", i + 1);
		if(pthread_detach(thread_list[i]) != 0) {
			delete[] thread_list;
			throw exception();
		}
	}
}

template<typename T>
thread_pool<T>::~thread_pool() {
	delete[] thread_list;
	thread_stop = true;
}

template<typename T>
bool thread_pool<T>::append(T *request) {
	// 上锁
	lock_worklist.lock();
	//判断能否入队
	if(list_work.size() > request_max) {
		lock_worklist.unlock();
		return false;
	}
	//入队
	list_work.push_back(request);
	//解锁
	lock_worklist.unlock();
	stat_list.post();
	return true;
}

template<typename T>
void *thread_pool<T>::child_thread_worker(void *arg) {
	thread_pool<T> * this_pool = (thread_pool<T> *) arg;
	this_pool->run();
	return this_pool;
}

template<typename T>
void thread_pool<T>::run() {
	while(!thread_stop) {
		stat_list.wait();
		lock_worklist.lock();
		if(list_work.empty()) {
			lock_worklist.unlock();
			continue;
		}

		T * request = list_work.front();
		list_work.pop_front();
		lock_worklist.unlock();

		if(!request) {
			continue;
		}
		request->process();
	}
}

#endif




