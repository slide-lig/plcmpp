
#pragma once

#include <thread>
#include <map>
#include <string>
using namespace std;

#include <io/PatternsCollector.hpp>

namespace io {

class FileCollector;

/**
 * A thread safe PatternsCollector that will write to multiple files, one per mining thread.
 */
class MultiThreadedFileCollector
    : public PatternsCollector
{

private:
    map<thread::id, FileCollector*> *collectors;
    string _prefix;

    FileCollector *getCollectorOfCurrentThread();

public:
    void collect(int32_t support, vector<int32_t>* pattern) override;
    int64_t close() override;
    int32_t getAveragePatternLength() override;

	/**
	 * @param prefix
	 * 			filename prefix for pattern files, each thread will append [ThreadID].dat
	 */
    MultiThreadedFileCollector(string& prefix);
    ~MultiThreadedFileCollector();
};

}
