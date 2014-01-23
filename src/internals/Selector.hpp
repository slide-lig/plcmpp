
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
public:
	/**
	 * Thrown when a Selector finds that an extension won't be the first parent
	 * of its closed pattern (FirstParentTest should be the only one concerned)
	 */

    class WrongFirstParentException /* : public exception */
    {
    public:
        int32_t firstParent;
        int32_t extension;

    public:
		/**
		 * @param extension
		 *            the tested extension
		 * @param foundFirstParent
		 *            a item found in closure > extension
		 */
        WrongFirstParentException(
        		int32_t exploredExtension, int32_t foundFirstParent);
     };

protected:
    /**
	 * @param extension in state's local base
	 * @param state
	 * @return false if, at the given state, trying to extend the current
	 *         pattern with the given extension is useless
	 * @throws WrongFirstParentException
	 */
    virtual bool allowExploration(int32_t extension, ExplorationStep* state)
    		throw (Selector::WrongFirstParentException) = 0;

public:
    virtual ~Selector();
    /**
	 * @param extension
	 * @param state
	 * @return false if, at the given state, trying to extend the current
	 *         pattern with the given extension is useless
	 * @throws WrongFirstParentException
	 */
    bool select(int32_t extension, ExplorationStep* state)
    				throw (Selector::WrongFirstParentException);

protected:
    virtual void incrementCounter() = 0;
};

}
