
#include <internals/FirstParentTest.hpp>
#include <internals/ExplorationStep.hpp>
#include <internals/Counters.hpp>
#include <internals/Dataset.hpp>
#include <util/shortcuts.h>

using namespace util;

namespace internals {

bool FirstParentTest::isAincludedInB(
		Iterator<int32_t>& aIt,
		Iterator<int32_t>& bIt) {

	int32_t tidA = 0;
	int32_t tidB = 0;

	while (aIt.hasNext() && bIt.hasNext()) {
		tidA = aIt.next();
		tidB = bIt.next();

		while (tidB < tidA && bIt.hasNext()) {
			tidB = bIt.next();
		}

		if (tidB > tidA) {
			return false;
		}
	}

	return tidA == tidB && !aIt.hasNext();
}

/**
 * returns true or throws a WrongFirstParentException
 */
bool FirstParentTest::allowExploration(int32_t extension,
		ExplorationStep* state)
{
	unique_ptr<Iterator<int32_t> > candidateOccurrences;
	unique_ptr<Iterator<int32_t> >  iOccurrences;
	auto supportCounts = state->counters->supportCounts->array;
	Dataset* d = state->dataset.get();

	int32_t candidateSupport = supportCounts[extension];
	int32_t i = state->counters->maxFrequent;
	supportCounts += i;

	for (; i > extension; i--, supportCounts--) {
		if (*supportCounts >= candidateSupport) {
			candidateOccurrences = d->getItemTidListIterator(extension);
			iOccurrences = d->getItemTidListIterator(i);
			if (isAincludedInB(*candidateOccurrences, *iOccurrences)) {
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

