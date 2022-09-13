#include "AbstractDispatcher.h"
#include <future>

class AsyncDispatcher : public AbstractDispatcher {
public:
	void execute(std::function<void()>&& func) {
		std::async(func);
	}
};
