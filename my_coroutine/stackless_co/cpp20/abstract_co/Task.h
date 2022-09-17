#pragma once
/// co包装体
#include <coroutine>
#include <iostream>


#include "../Awaiters/FutureAwaiter.h"
#include "../Awaiters/TaskAwaiter.h"
#include "../Awaiters/DispatchAwaiter.h"

#include "../Dispatchers/LooperDispatcher.h"
#include "../Dispatchers/NewThreadDispatcher.h"
#include "../Dispatchers/NoopDispatcher.h"
#include "../Dispatchers/AsyncDispatcher.h"


#include "Promise.h"

struct asd {

};

template<typename result_type, typename dispatcher_type = NoopDispatcher>
struct Task {

	using promise_type = Promise<result_type, dispatcher_type>;

	result_type get_result() {
		return handle.promise().get_result();
	}

	Task& then(std::function<void(result_type)>&& func) {
		handle.promise().on_completed([func](result_type result) {
			try {
				func(result.get_or_throw());
			}
			catch (std::exception& e) {
				// ignore.
			}
			});
		return *this;
	}

	Task& catching(std::function<void(std::exception&)>&& func) {
		handle.promise().on_completed([func](auto result) {
			try {
				result.get_or_throw();
			}
			catch (std::exception& e) {
				func(e);
			}
			});
		return *this;
	}

	Task& finally(std::function<void()>&& func) {
		handle.promise().on_completed([func](auto result) { func(); });
		return *this;
	}

	explicit Task(std::coroutine_handle<promise_type> handle) noexcept : handle(handle) {}

	Task(Task&& task) noexcept : handle(std::exchange(task.handle, {})) {}

	Task(Task&) = delete;

	~Task() {

	}

private:
	std::coroutine_handle<promise_type> handle;
};

