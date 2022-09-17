// co的promise_type
#pragma once
#include <coroutine>
#include <mutex>
#include <iostream>
#include <functional>
#include <optional>
#include <exception>
#include <type_traits>


#include "Result.h"

#include "../Awaiters/AbstractAwaiter.h"
#include "../Awaiters/DispatchAwaiter.h"
#include "../Awaiters/FutureAwaiter.h"
#include "../Awaiters/TaskAwaiter.h"
#include "../Awaiters/SleepAwaiter.h"

#include "../Dispatchers/NoopDispatcher.h"
#include "../Dispatchers/AsyncDispatcher.h"
#include "../Dispatchers/NewThreadDispatcher.h"
#include "../Dispatchers/LooperDispatcher.h"

template<typename result_type, typename dispatcher_type>
struct Task;

template<typename result_type, typename dispatcher_type>
class Promise {
	std::optional<Result<result_type>> result;

	std::mutex completion_lock;
	std::condition_variable completion;

	// 成功后的调用链
	std::list<std::function<void(Result<result_type>)>> completion_callbacks;

	dispatcher_type _dispathcher;
public:

	// 第一次使用调度器
	DispatchAwaiter<void> initial_suspend() {
		DispatchAwaiter<void> awaiter;
		awaiter.set_dispatcher(&_dispathcher);
		return awaiter;
	}

	//std::suspend_never	initial_suspend() {
	//	return {};
	//}

	std::suspend_always final_suspend() noexcept {
		return {};
	}

	Task<result_type, dispatcher_type> get_return_object() {
		return Task{ std::coroutine_handle<Promise>::from_promise(*this) };
	}

	template<typename result_type>
	TaskAwaiter<result_type, dispatcher_type> await_transform(Task<result_type, dispatcher_type>&& task) {
		return await_transform(TaskAwaiter<result_type, dispatcher_type>(std::move(task)));
	}
	template<typename R, typename P>
	 auto await_transform(std::chrono::duration<R, P> duration) {
		 return await_transform(SleepAwaiter(duration));
	}

	// 增加校验
	template<typename AwaiterImpl, typename std::enable_if_t<std::is_base_of_v<AbstractAwaiter<result_type>, AwaiterImpl>>>
	AwaiterImpl await_transform(AwaiterImpl awaiter_impl) {
		awaiter_impl.set_dispatcher(&_dispathcher);
		return awaiter_impl;
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