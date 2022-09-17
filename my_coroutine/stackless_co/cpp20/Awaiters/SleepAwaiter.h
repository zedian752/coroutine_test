#pragma once


#include "../helpful_components/Timer.h"

#include "AbstractAwaiter.h"

template<typename R>
class SleepAwaiter : public AbstractAwaiter<void> {
	std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
	static Timer timer;
public:

	template<typename R, typename P>
	explicit SleepAwaiter(std::chrono::duration<R, P> duration) : time_point(std::chrono::high_resolution_clock::now() + duration) {
	}
	SleepAwaiter(SleepAwaiter&& rhs) = delete;

	SleepAwaiter(SleepAwaiter& rhs) = delete;
	SleepAwaiter& operator=(SleepAwaiter& rhs) = delete;

	void before_suspend() override {
		// 通过SleepAwaiter内置的定时器来进行调度
		timer.addTimePointFun(time_point, [this] {
			// 触发dispatcher的调度
			this->resume();
		});
	}
};