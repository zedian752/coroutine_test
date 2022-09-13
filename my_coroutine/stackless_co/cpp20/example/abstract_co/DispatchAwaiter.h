#pragma once
#include <coroutine>

#include "../dispatcher/AbstractDispatcher.h"
#include <coroutine>

class DispatchAwaiter {
	AbstractDispatcher * _dispatcher;
public:
    DispatchAwaiter(AbstractDispatcher* dispatcher) noexcept:_dispatcher(dispatcher) {}

    bool await_ready() const { return false; }

    void await_suspend(std::coroutine_handle<> handle) const {

            _dispatcher->execute([handle]() {
                handle.resume();
            });
    }

    void await_resume() {}
};