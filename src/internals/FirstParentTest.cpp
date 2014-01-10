
#include <internals/FirstParentTest.hpp>
#include <internals/ExplorationStep.hpp>
#include <internals/Counters.hpp>
#include <internals/Dataset.hpp>
#include <util/shortcuts.h>

using namespace util;

namespace internals {

PLCM::PLCMCounters FirstParentTest::getCountersKey() {
	return PLCM::PLCMCounters::FirstParentTestRejections;
}

unique_ptr<Selector> FirstParentTest::copy() {
	return unique_ptr<Selector>(new FirstParentTest());
}

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
		ExplorationStep* state) throw (Selector::WrongFirstParentException)
{
	shp_vec_int32 supportCounts = state->counters->supportCounts;
	Dataset* d = state->dataset.get();

	int32_t candidateSupport = (*supportCounts)[extension];

	for (int32_t i = state->counters->maxFrequent; i > extension; i--) {
		if ((*supportCounts)[i] >= candidateSupport) {
			unique_ptr<Iterator<int32_t> > candidateOccurrences =
					d->getTidList(extension);
			unique_ptr<Iterator<int32_t> >  iOccurrences =
					d->getTidList(i);
			if (isAincludedInB(*candidateOccurrences, *iOccurrences)) {
				PLCM::getCurrentThread()->counters[
				            PLCM::PLCMCounters::FirstParentTestRejections]++;
				throw Selector::WrongFirstParentException(extension, i);
			}
		}
	}

	return true;
}

}

