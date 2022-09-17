#pragma once
#include <thread>
#include <chrono>

#include "AbstractAwaiter.h"
#include "Task.h"
#include "Promise.h"

namespace abstract_co_test {
	Task<int>t1() {
		std::cout << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		co_return 10086;
	}

	Task<int> _get() {
		auto c = co_await t1();
		co_return 1;
	}

	void test() {
		Task<int> c  = _get();
		c.get_result();
	}
}
