#pragma once

#include <util/PollableThread.hpp>
#include <atomic>
#include <sys/resource.h>

namespace util {

class MemoryPeakWatcherThread : public PollableThread
{
protected:
	std::atomic<long> maxUsedMemory;

	static struct rusage rusage_holder;

private:
	static const long CHECK_DELAY;
	void onPoll(bool timeout);

public:
	MemoryPeakWatcherThread();
	~MemoryPeakWatcherThread();

	long getMaxUsedMemory();
};

}
