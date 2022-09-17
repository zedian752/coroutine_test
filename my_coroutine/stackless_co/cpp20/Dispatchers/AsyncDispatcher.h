#pragma once
#include "AbstractDispatcher.h"
#include <future>
#include <queue>
#include <functional>

class AsyncDispatcher : public AbstractDispatcher {
public:
	void execute(std::function<void()>&& func) {
		std::async(func);
	}
	~AsyncDispatcher() {

	}
};
