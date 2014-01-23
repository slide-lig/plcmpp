
#include <internals/Selector.hpp>

namespace internals {

Selector::~Selector() {
}

bool Selector::select(int32_t extension, ExplorationStep* state)
		throw (Selector::WrongFirstParentException) {
	if (allowExploration(extension, state)) {
		return true;
	} else {
		incrementCounter();
		return false;
	}
}

Selector::WrongFirstParentException::WrongFirstParentException(
		int32_t exploredExtension, int32_t foundFirstParent) {
	firstParent = foundFirstParent;
	extension = exploredExtension;
}

}

