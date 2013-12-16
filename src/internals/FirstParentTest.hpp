

#pragma once

#include <internals/Selector.hpp>
#include <PLCM.hpp>

#include <vector>
using namespace std;

namespace internals {

class FirstParentTest : public Selector
{

protected:
    FirstParentTest();
    FirstParentTest(Selector* follower);

    PLCM::PLCMCounters getCountersKey() override;

    unique_ptr<Selector> copy() override;
    bool allowExploration(int32_t extension, ExplorationStep* state) /* throws(WrongFirstParentException) */ override;

private:
    bool isAincludedInB(const vector<int32_t>::iterator& aIt,
    		const vector<int32_t>::iterator& bIt);
};

}
