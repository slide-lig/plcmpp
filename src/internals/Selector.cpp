
#include <internals/Selector.hpp>

namespace internals {

Selector::~Selector() {
}

bool Selector::select(int32_t extension, ExplorationStep* state) {
	if (allowExploration(extension, state)) {
		return true;
	} else {
		incrementCounter();
		return false;
	}
}

}

