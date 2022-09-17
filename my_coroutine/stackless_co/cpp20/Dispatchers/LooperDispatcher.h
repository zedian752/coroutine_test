#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <functional>
#include <atomic>

#include "AbstractDispatcher.h"

class LooperDispatcher: public AbstractDispatcher {
	std::condition_variable _queue_cv;
	std::mutex _queue_mtx;
	
	std::queue<std::function<void()>> _excutable_queue;
	std::atomic<bool> _is_active{true};
	/* 工作线程 */
	std::thread _work_thread;
	void _init() {
		_work_thread = std::thread(&LooperDispatcher::_run_loop, this);
	};

	void _run_loop() {
		//while (_is_active.load(std::memory_order_relaxed)) {
		//	std::unique_lock<std::mutex> lock(_queue_mtx);

		//	if (_excutable_queue.empty()) {
		//		_queue_cv.wait(lock);
		//		if (_excutable_queue.empty()) {
		//			continue;
		//		}
		//	}

		//	// 饱和式完成任务
		//	while (!_excutable_queue.empty()) {
		//		auto func = _excutable_queue.front();
		//		_excutable_queue.pop();
		//		func();
		//	}
		//	lock.unlock();
		//}
	}

public:

	LooperDispatcher() {
		_init();
	}

	~LooperDispatcher() noexcept {
		this->shutdown();
	}


	void execute(std::function<void()>&& func) {
		if (!this->_is_active.load(std::memory_order_relaxed)) {
			return;
		}
		std::unique_lock<std::mutex> lock(_queue_mtx);
		_excutable_queue.push(func);
		lock.unlock();
		_queue_cv.notify_one();
	}

	void shutdown() {
		std::unique_lock<std::mutex> lock(_queue_mtx);
		_is_active.store(false, std::memory_order_relaxed);
		lock.unlock();
		if (_work_thread.joinable()) {
			_work_thread.join();
		}
	}
}; 
