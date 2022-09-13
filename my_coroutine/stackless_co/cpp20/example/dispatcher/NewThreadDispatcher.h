#include "AbstractDispatcher.h"
#include <thread>

class NewThreadDispatcher : public AbstractDispatcher {
public:
	void execute(std::function<void()>&& func) {
		std::thread(func).detach();
	}
};
