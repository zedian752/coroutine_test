#include <iostream>
#include <chrono>

#include "../abstract_co/Task.h"
#include "NoopDispatcher.h"
#include "AsyncDispatcher.h"

namespace dispatcher_test {

	Task<int, NoopDispatcher> noop_dispatcher_test() {
		std::cout << "task start" << std::endl;
		cout << std::chrono::system_clock::now() << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		cout << std::chrono::system_clock::now() << endl;
		std::cout << "task end" << std::endl;
		co_return 2;
	}

	Task<int, AsyncDispatcher> async_dispatcher_test() {
		std::cout << "task start" << std::endl;
		cout << std::chrono::system_clock::now() << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		cout << std::chrono::system_clock::now() << endl;
		std::cout << "task end" << std::endl;
		co_return 2;
	}


	void test() {
		async_dispatcher_test();
		//noop_dispatcher_test();
	}

}