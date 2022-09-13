#pragma once
/// co包装体
#include <coroutine>
#include <iostream>
template<typename result_type, typename dispatcher_type>
class _Promise;

#include "Awaiter.h"
#include "NewThreadDispatcher.h"


template<typename result_type, typename dispatcher_type = NewThreadDispatcher>
struct _Task {

	using promise_type = _Promise<result_type>;

	result_type get_result() {
		return handle.promise().get_result();

	}

	_Task& then(std::function<void(result_type)>&& func) {
		handle.promise().on_completed([func](auto result) {
			try {
				func(result.get_or_throw());
			}
			catch (std::exception& e) {
				// ignore.
			}
			});
		return *this;
	}

	_Task& catching(std::function<void(std::exception&)>&& func) {
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

	_Task& finally(std::function<void()>&& func) {
		handle.promise().on_completed([func](auto result) { func(); });
		return *this;
	}

	explicit _Task(std::coroutine_handle<promise_type> handle) noexcept : handle(handle) {}

	_Task(_Task&& task) noexcept : handle(std::exchange(task.handle, {})) {}

	_Task(_Task&) = delete;

	~_Task() {

	}

	

private:
	std::coroutine_handle<promise_type> handle;
};

