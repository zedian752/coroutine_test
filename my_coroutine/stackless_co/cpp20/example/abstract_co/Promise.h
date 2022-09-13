
// co的promise_type
#include <coroutine>
#include <mutex>
#include <iostream>
#include <functional>
#include <optional>
#include <exception>
#include "DispatchAwaiter.h"
#include "Awaiter.h"

template<typename T>
struct Result {

	explicit Result() = default;

	explicit Result(T&& value) : _value(value) {}

	explicit Result(std::exception_ptr&& exception_ptr) : _exception_ptr(exception_ptr) {}

	T get_or_throw() {
		if (_exception_ptr) {
			std::rethrow_exception(_exception_ptr);
		}
		return _value;
	}

private:
	T _value{};
	std::exception_ptr _exception_ptr;
};

template<typename ResultType, typename dispatcher_type>
class _Task;

template<typename result_type, typename dispatcher_type>
class _Promise {
	std::optional<Result<result_type>> result;

	std::mutex completion_lock;
	std::condition_variable completion;

	// 成功调用链
	std::list<std::function<void(Result<result_type>)>> completion_callbacks;

	dispatcher_type dispathcher;
public:
	DispatchAwaiter initial_suspend() { return DispatchAwaiter{&dispathcher}; }
	std::suspend_always final_suspend() noexcept {
		return {};
	}

	_Task<result_type, dispatcher_type> get_return_object() {
		return _Task{ std::coroutine_handle<_Promise>::from_promise(*this) };
	}

	template<typename result_type>
	Awaiter<result_type> await_transform(_Task<result_type>&& task) {
		cout << "await_transform" << endl;
		return Awaiter<result_type, dispatcher_type>(&dispathcher, std::move(task));
	}

	void unhandled_exception() {
		std::lock_guard lock(completion_lock);
		result = Result<result_type>(std::current_exception());
		completion.notify_all();
		notify_callbacks();
	}

	void return_value(result_type value) {
		std::lock_guard lock(completion_lock);
		result = Result<result_type>(std::move(value));
		completion.notify_all();
		notify_callbacks();
	}

	result_type get_result() {
		std::unique_lock lock(completion_lock);
		
		if (!result.has_value()) {
			completion.wait(lock);
		}
		return result->get_or_throw();

	}

	void on_completed(std::function<void(Result<result_type>)>&& fun) {
		std::unique_lock lock(completion_lock);
		if (result.has_value()) {
			auto value = result.value();
			lock.unlock();
			fun(value);
		}
		else {
			completion_callbacks.push_back(fun);
		}

	}

	void notify_callbacks() {
		auto value = result.value();
		for (auto& callback : completion_callbacks) {
			try {
				callback(value);

			} catch(const std::exception& e) {
				cout << e.what() << endl;
			  }
		}
		completion_callbacks.clear();
	}

};