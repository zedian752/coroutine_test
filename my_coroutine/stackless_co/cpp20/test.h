
#include <iostream>
#include <coroutine>
#include "abstract_co/Task.h"

using namespace std;

Task<int> test_return() {
	co_return 1;

}


void test_enter() {
	
	auto task = test_return();
	cout << task.get_result() << endl;;
 
}