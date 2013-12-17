
#pragma once

#include "PLCM.hpp"

#include <exception>
#include <deque>
using namespace std;

namespace internals {

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
    virtual bool allowExploration(int32_t extension, ExplorationStep* state) = 0;

	/**
	 * @return an instance of the same selector for another recursion
	 */
    virtual unique_ptr<Selector> copy() = 0;

	/**
	 * @return which enum value from TopLCMCounters will be used to count this Selector's rejections,
	 * or null if we don't want to count rejections from the implementing class
	 */
    virtual PLCM::PLCMCounters getCountersKey() = 0;

protected:
	/**
	 * @param extension
	 * @param state
	 * @return false if, at the given state, trying to extend the current
	 *         pattern with the given extension is useless
	 * @throws WrongFirstParentException
	 */
    bool select(int32_t extension, ExplorationStep* state);

public:
    virtual ~Selector();

	/**
	 * Thrown when a Selector finds that an extension won't be the first parent
	 * of its closed pattern (FirstParentTest should be the only one concerned)
	 */

    class WrongFirstParentException : public exception
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
        WrongFirstParentException(int32_t exploredExtension, int32_t foundFirstParent);
    };

    class List : public deque<unique_ptr<Selector>> {
    	unique_ptr<List> copy();

    	bool select(int32_t extension, ExplorationStep* state);
    };
};

}
