

#pragma once

#include <internals/Selector.hpp>
#include <util/Iterator.hpp>
#include <PLCM.hpp>
using util::Iterator;

namespace internals {

class FirstParentTest : public Selector
{

protected:
	PLCM::PLCMCounters getCountersKey() override;

    unique_ptr<Selector> copy() override;
    bool allowExploration(int32_t extension, ExplorationStep* state)
    		throw (Selector::WrongFirstParentException) override;

private:
    bool isAincludedInB(Iterator<int32_t>& aIt,
    					Iterator<int32_t>& bIt);
};

}
