
#include <internals/FirstParentTest.hpp>
#include <internals/ExplorationStep.hpp>
#include <internals/Counters.hpp>
#include <internals/Dataset.hpp>
#include <util/shortcuts.h>

using namespace util;

namespace internals {

/**
 * check if we should explore this extension
 */
bool FirstParentTest::allowExploration(int32_t extension,
		ExplorationStep* state)
{
	auto supportCounts = state->counters->supportCounts->array;
	Dataset* d = state->dataset.get();

	int32_t candidateSupport = supportCounts[extension];
	int32_t i = state->counters->maxFrequent;
	supportCounts += i;

	for (; i > extension; i--, supportCounts--) {
		if (*supportCounts >= candidateSupport) {
			if (d->checkTidListsInclusion(extension, i))
			{
				PLCM::getCurrentThread()->counters[
				            PLCM::PLCMCounters::FirstParentTestRejections]++;
				state->addFailedFPTest(extension, i);
				return false;
			}
		}
	}

	return true;
}

}

