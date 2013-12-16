
#include "util/MemoryPeakWatcherThread.hpp"

namespace util {

// peek delay, in milliseconds
const long MemoryPeakWatcherThread::CHECK_DELAY = 15000;

struct rusage MemoryPeakWatcherThread::rusage_holder;

MemoryPeakWatcherThread::MemoryPeakWatcherThread() :
						PollableThread(CHECK_DELAY) {
	maxUsedMemory = 0;
}

MemoryPeakWatcherThread::~MemoryPeakWatcherThread() {
}

void MemoryPeakWatcherThread::onPoll(bool timeout) {
	getrusage(RUSAGE_SELF, &rusage_holder);
	if (rusage_holder.ru_maxrss > maxUsedMemory)
	{
		maxUsedMemory = rusage_holder.ru_maxrss;
	}
}

long MemoryPeakWatcherThread::getMaxUsedMemory() {
	return maxUsedMemory;
}

}

