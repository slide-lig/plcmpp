

#pragma once

#include <internals/Selector.hpp>
#include <util/Iterator.hpp>
#include <PLCM.hpp>
using util::Iterator;

namespace internals {

class FirstParentTest : public Selector
{
protected:
    bool allowExploration(
    		int32_t extension, ExplorationStep* state)
    		override;

    inline void incrementCounter() override {
    		PLCM::getCurrentThread()->counters[
    		          PLCM::PLCMCounters::FirstParentTestRejections]++;
    }
};

}
