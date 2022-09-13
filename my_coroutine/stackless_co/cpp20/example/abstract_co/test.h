#pragma once
#include <thread>
#include <chrono>

#include "Awaiter.h"
#include "Task.h"
#include "Promise.h"

namespace abstract_co {
	_Task<int>t1() {
		cout << std::this_thread::get_id() << endl;
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		
		co_return 10086;
	}




	_Task<int> _get() {
		auto c = co_await t1();
		//cout << c << endl;
		co_return 1;
		 
	}


	void test() {
	
		_get();

	}
}
