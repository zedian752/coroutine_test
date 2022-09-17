

#pragma once


#include "AbstractAwaiter.h"


template<typename R>
class DispatchAwaiter : public AbstractAwaiter<R> {
public:
	explicit DispatchAwaiter() {

	}
	DispatchAwaiter(DispatchAwaiter&& rhs) {
	}

	DispatchAwaiter(DispatchAwaiter& rhs) = delete;
	DispatchAwaiter& operator=(DispatchAwaiter& rhs) = delete;

	void before_suspend() override {
		this->resume();
	}
};