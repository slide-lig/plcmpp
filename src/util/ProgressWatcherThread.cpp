#include "util/ProgressWatcherThread.hpp"
#include "internals/ExplorationStep.hpp"

#include <ctime>
#include <iostream>
#include <iomanip>

using namespace std;

namespace util {

// delay, in milliseconds
const long ProgressWatcherThread::PRINT_STATUS_EVERY = 5 * 60 * 1000;

void util::ProgressWatcherThread::onPoll(bool timeout) {
	internals::Progress *progress = _step->getProgression();

	time_t t = std::time(nullptr);
    tm tm = *std::localtime(&t);

    // not very beautiful, but libstd++ does not implement std::put_time() yet.
    char formatted_time[20];
    std::strftime(formatted_time, sizeof(formatted_time), "%F %T", &tm);

    cerr << formatted_time << " - root iterator state : " <<
    		progress->current << "/" << progress->last << endl;
}

void util::ProgressWatcherThread::setInitState(internals::ExplorationStep* step) {
	_step = step;
}

}
