
#include "util/MemoryUsage.hpp"
#include "Config.hpp"

#include <unistd.h>
#include <iostream>
#include <fstream>
using namespace std;

namespace util {
namespace MemoryUsage {

WatcherThread::WatcherThread() :
						PollableThread(Config::MEMORY_USAGE_CHECK_DELAY) {
	_kbytes_per_page = getpagesize() / 1024;
	cout << "kbytes_per_page = " << _kbytes_per_page << endl;
}

WatcherThread::~WatcherThread() {
}

void WatcherThread::onPoll(bool timeout) {
	printRSS();
}

void WatcherThread::onInit() {
	printRSS();
}

void WatcherThread::printRSS() {
	ifstream reader("/proc/self/statm");
	uint i;
	reader >> i;
	reader >> i; // second integer is the RSS (resident set size)
	cout << "Memory (RSS): " << i * _kbytes_per_page << "k" << endl;
}

long getPeakMemoryUsage() {
	static struct rusage rusage_holder;
	getrusage(RUSAGE_SELF, &rusage_holder);
	return rusage_holder.ru_maxrss;
}

}
}

