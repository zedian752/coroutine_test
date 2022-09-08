#ifndef serial_number_generator
#define serial_number_generator
#include <coroutine>

struct Generator {

public:
	
	struct promise_type {
		int value;

		Generator get_return_object() {
			return Generator{ std::coroutine_handle<promise_type>::from_promise(*this) };
		}
		// 开始执行时直接挂起等待外部调用 resume 获取下一个值
		std::suspend_always initial_suspend() { return {}; };

		// 执行结束后不需要挂起
		std::suspend_never final_suspend() noexcept { return {}; }

		std::suspend_always await_transform(int value) {
			this->value = value;
			return {};
		}
		int return_value(int value) {
			return value;
		}

		 
		// 为了简单，我们认为序列生成器当中不会抛出异常，这里不做任何处理
		void unhandled_exception() { }
	};
	std::coroutine_handle<promise_type> handle;

	int next() {
		handle.resume();

		// 外部调用者或者恢复者可以通过读取 value
		return handle.promise().value;
	}
};

static Generator sequence() {
	int i = 0;
	while (true) {
		co_await i++;
	}
}


#endif // !serial_number_generator
