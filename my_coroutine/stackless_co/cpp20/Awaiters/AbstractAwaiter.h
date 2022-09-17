
// 为了更好地控制co_await的功能
#pragma once
#include <coroutine>
#include <optional>
#include <memory>
#include <functional>

#include "Result.h"
#include "AbstractDispatcher.h"

template<typename result_type, typename dispatcher>
struct Task;

///
/// 泛型R用于co_await之后的返回值
///
template<typename R>
class AbstractAwaiter {
    std::coroutine_handle<> _handle;
    //std::shared_ptr<AbstractDispatcher> _dispatcher;
    AbstractDispatcher* _dispatcher = nullptr;
protected:
    std::optional<Result<R>> _result{};
        // 生命周期hook:协程暂停前, 在这里用异步方法恢复协程
    virtual void before_suspend() {}
    // 生命周期hook:协程暂停后
    virtual void before_resume() {}

    // 调度
    void dispatch(std::function<void()>&& f) {
        if (_dispatcher) {
            _dispatcher->execute(std::move(f));
        }
        else {
            f();
        }
    }
public:

    using result_type = R;
    constexpr bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        this->_handle = handle;
        this->before_suspend();
    }

    //R await_resume()  {
    R await_resume() noexcept {
        this->before_resume();
        return _result->get_or_throw();
    }

    // 子类的before_suspend中调用该函数实现协程恢复
    void resume_value(R value) {
        dispatch([this, value] {
            _result = Result<R>(value);
            _handle.resume();
        });
    }
    void resume_exception(std::exception_ptr && e) {
        dispatch([this, e] {
            _result = Result<R>(e);
            _handle.resume();
         });
    }

    // 不提供 value，但也要恢复协程，这种情况需要子类在 before_resume 当中写入 _result，或者抛出异常
    void resume() {
        // 传递resume权给dispatcher
        dispatch([this] {
            _handle.resume();
        });
    }

    void set_dispatcher(AbstractDispatcher * dispatcher) {
        this->_dispatcher = dispatcher;
    }
};




template<>
class AbstractAwaiter<void> {
    std::coroutine_handle<> _handle;
    AbstractDispatcher * _dispatcher = nullptr;
protected:
    std::optional<Result<void>> _result;
    // 生命周期hook:协程暂停前, 在这里用异步方法恢复协程
    virtual void before_suspend() {}
    // 生命周期hook:协程暂停后
    virtual void before_resume() {}

    // 调度
    void dispatch(std::function<void()>&& f) {
        if (_dispatcher) {
            _dispatcher->execute(std::move(f));
        }
        else {
            f();
        }
    }
public:

    using result_type = void;
    constexpr bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        _handle = handle;
        before_suspend();
    }

    void await_resume() noexcept {
        before_resume();
        _result->throw_error();
    }

    void resume() {
        
        dispatch([this] {
            _result = Result<void>();
            _handle.resume();
        });
    }
    void resume_exception(std::exception_ptr&& e) {
        dispatch([this, e] {
            // TODO handle
            _result = Result<void>(static_cast<std::exception_ptr>(e));
            _handle.resume();
        });
    }


    void set_dispatcher(AbstractDispatcher* dispatcher) {
        _dispatcher = dispatcher;
    }
};