#pragma once

#include <future>
#include <thread>

#include "AbstractAwaiter.h"

template<typename R>
class FutureAwaiter : public AbstractAwaiter<R> {
	std::future<R> _future;
public:
	explicit FutureAwaiter(std::future<R> future): _future(future) {
		
	}
	FutureAwaiter(FutureAwaiter&& rhs) : _future(std::move(rhs._future)) {
	}

	FutureAwaiter(FutureAwaiter& rhs) = delete;
	FutureAwaiter& operator=(FutureAwaiter& rhs) = delete;

	void before_suspend() override {
		std::thread([this] {
			this->resume_value(_future.get());
		}).detach();
	}
};