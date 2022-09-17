#pragma once
#include "AbstractAwaiter.h"

template<typename result_type, typename dispatcher>
struct Task;

template<typename result_type, typename dispatcher>
class TaskAwaiter : public AbstractAwaiter<result_type> {
	Task<result_type, dispatcher> _task;
public:
	explicit TaskAwaiter(Task<result_type, dispatcher> task) : _task(task) {
	}
	TaskAwaiter(TaskAwaiter&& rhs) : _task(std::move(rhs._task)) {
		
	}

	TaskAwaiter(TaskAwaiter& rhs) = delete;
	TaskAwaiter & operator=(TaskAwaiter& rhs) = delete;


	void before_suspend() override {
		_task.finally([this] {
			// 恢复本协程
			this->resume();
		});
	}

	void before_resume() override {
		this->_result = Result(_task.get_result());
	}
};