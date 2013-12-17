
#pragma once

#include <vector>
#include <map>
#include <mutex>
#include <deque>
#include <cstdint>
#include <iostream>
#include <thread>
using namespace std;

#include "io/PatternsCollector.hpp"
#include "util/tclap/CmdLine.h"

using io::PatternsCollector;

class PLCMThread;

namespace util {
class ProgressWatcherThread;
}

namespace internals {
class ExplorationStep;
}

using internals::ExplorationStep;

class PLCM
{
public:
	enum PLCMCounters {
			ExplorationStepInstances,
			ExplorationStepCatchedWrongFirstParents,
			FirstParentTestRejections,
			TransactionsCompressions,
			Number_of_PLCMCounters
	};

protected:
    deque<PLCMThread>* threads;

private:
    util::ProgressWatcherThread* progressWatch;
    static int64_t chrono;
    PatternsCollector* collector;
    int64_t globalCounters[PLCMCounters::Number_of_PLCMCounters];

public:
    PLCM(PatternsCollector* patternsCollector, int32_t nbThreads);
    ~PLCM();

    void collect(int32_t support, vector<int32_t>* pattern);
    void lcm(ExplorationStep* initState);
    unique_ptr<map<PLCMCounters, uint64_t> > getCounters();
    void display(ostream& stream, map<PLCMCounters, uint64_t>* additionalCounters);

    static int main(int argc, char**argv);
    static void printMan();
    static void standalone(TCLAP::CmdLine& cmd);
    static PLCMThread* getCurrentThread();

protected:
    void createThreads(int32_t nbThreads);
    void initializeAndStartThreads(ExplorationStep* initState);
    ExplorationStep* stealJob(PLCMThread* thief);

    static ExplorationStep* stealJob(PLCMThread* thief, PLCMThread* victim);

private:
    static unique_ptr<PatternsCollector> instanciateCollector(
    		TCLAP::CmdLine& cmd, string& outputPath, int32_t nbThreads);
};

class PLCMThread
{
protected:
	mutex _mutex;
	unique_lock<mutex> *_lock;
	thread* _thread;
    deque<ExplorationStep*>* stackedJobs;
    thread::id id;
    PLCM *_PLCM_instance;

public:
    uint64_t counters[PLCM::PLCMCounters::Number_of_PLCMCounters];

public:
    PLCMThread(PLCM *PLCM_instance, int32_t id);
    thread::id getId();
    void run();

protected:
    void init(ExplorationStep* initState);

private:
    void lcm(ExplorationStep* state);
};

