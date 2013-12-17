
#include <internals/Selector.hpp>

namespace internals {

Selector::~Selector() {
}

bool Selector::select(int32_t extension, ExplorationStep* state) {
	if (allowExploration(extension, state)) {
		return true;
	} else {
		PLCM::PLCMCounters key = getCountersKey();
		PLCM::getCurrentThread()->counters[key]++;
		return false;
	}
}

Selector::WrongFirstParentException::WrongFirstParentException(
		int32_t exploredExtension, int32_t foundFirstParent) {
	firstParent = foundFirstParent;
	extension = exploredExtension;
}

unique_ptr<Selector::List> Selector::List::copy() {

	unique_ptr<Selector::List> cloned(new Selector::List());

	for (auto it = this->begin(); it != this->end(); ++it)
	{
		cloned->push_back((*it)->copy());
	}

	return cloned;
}

bool Selector::List::select(int32_t extension, ExplorationStep* state)
{
	for (auto it = this->begin(); it != this->end(); ++it)
	{
		if (!(*it)->allowExploration(extension, state))
		{
			return false;
		}
	}

	return true;
}

}

