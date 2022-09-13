#include "AbstractDispatcher.h"

class NoopDispatcher : public AbstractDispatcher {

public:
	void execute(std::function<void()>&& func) {
		func();
	}
};
