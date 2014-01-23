
#pragma once

#include "PLCM.hpp"

#include <exception>
#include <deque>
using namespace std;

namespace internals {

class ExplorationStep;

/**
 * Main class for chained exploration filters, implemented as an immutable
 * chained list.
 */
class Selector
{
protected:
    /**
	 * @param extension in state's local base
	 * @param state
	 * @return false if, at the given state, trying to extend the current
	 *         pattern with the given extension is useless
	 * @throws WrongFirstParentException
	 */
    virtual bool allowExploration(
    		int32_t extension, ExplorationStep* state) = 0;

public:
    virtual ~Selector();
    /**
	 * @param extension
	 * @param state
	 * @return false if, at the given state, trying to extend the current
	 *         pattern with the given extension is useless
	 * @throws WrongFirstParentException
	 */
    bool select(int32_t extension, ExplorationStep* state);

protected:
    virtual void incrementCounter() = 0;
};

}
