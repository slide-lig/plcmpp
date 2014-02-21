#pragma once

#include <util/PollableThread.hpp>
#include <atomic>
#include <sys/resource.h>

namespace util {
namespace MemoryUsage {

class WatcherThread : public PollableThread
{
private:
	uint _kbytes_per_page;

	void onInit() override;
	void onPoll(bool timeout) override;
	void printRSS();

public:
	WatcherThread();
	~WatcherThread();
};

long getPeakMemoryUsage();

}
}
