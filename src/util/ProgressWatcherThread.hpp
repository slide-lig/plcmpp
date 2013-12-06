#pragma once

#include <util/PollableThread.hpp>
#include <atomic>

namespace internals {
	class ExplorationStep;
}

/**
 * This thread will give some information about the progression on stderr every
 * 5 minutes. When running on Hadoop it may also be used to poke the master node
 * every 5 minutes so it doesn't kill the task.
 *
 * you MUST use setInitState before starting the thread you MAY use
 * setHadoopContext
 */

namespace util {

class ProgressWatcherThread : public PollableThread
{
private:
	static const long PRINT_STATUS_EVERY;
	internals::ExplorationStep *_step;

	void onPoll(bool timeout);

public:
	void setInitState(internals::ExplorationStep* step);
};

}
