#pragma once
#include <functional>

class AbstractDispatcher {
public:
	virtual void execute(std::function<void()>&& func) = 0;

	virtual ~AbstractDispatcher() {
	}
};