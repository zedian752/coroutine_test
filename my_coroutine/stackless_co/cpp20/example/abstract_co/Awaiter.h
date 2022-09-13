
// 为了更好地控制co_await的功能
#pragma once
#include <coroutine>

template<typename result_type, typename dispatcher>
struct _Task;

template<typename result_type, typename dispatcher>
struct Awaiter {
    explicit Awaiter(_Task<result_type, dispatcher>&& task) noexcept
        : task(std::move(task)) {}

    Awaiter(Awaiter&& completion) noexcept
        : task(std::exchange(completion.task, {})) {}

    Awaiter(Awaiter&) = delete;

    Awaiter& operator=(Awaiter&) = delete;

    constexpr bool await_ready() const noexcept {
        cout << "await_ready" << endl;
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        cout << "await_suspend" << endl;
        task.finally([handle]() {
            handle.resume();
            });
    }

    result_type await_resume() noexcept {
        cout << "await_resume" << endl;
        return task.get_result();
    }

private:
    _Task<result_type, dispatcher> task;
};