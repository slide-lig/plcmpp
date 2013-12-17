#include "util/ProgressWatcherThread.hpp"
#include "util/Helpers.h"
#include "internals/ExplorationStep.hpp"
using internals::ExplorationStep;
using internals::Progress;

#include <iostream>

using namespace std;

namespace util {

// delay, in milliseconds
const long ProgressWatcherThread::PRINT_STATUS_EVERY = 5 * 60 * 1000;

void ProgressWatcherThread::onPoll(bool timeout) {
	unique_ptr<Progress> progress = _step->getProgression();

    cerr << Helpers::formatted_time() << " - root iterator state : " <<
    		progress->current << "/" << progress->last << endl;
}

void ProgressWatcherThread::setInitState(ExplorationStep* step) {
	_step = step;
}

}
