/*******************************************************************************
 * Copyright (c) 2014 Etienne Dublé, Martin Kirchgessner, Vincent Leroy, Alexandre Termier, CNRS and Université Joseph Fourier.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the LICENSE.txt file joined with this program.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

#pragma once

#include <vector>
#include <map>
#include <mutex>
#include <deque>
#include <cstdint>
#include <iostream>
#include <thread>
#include <condition_variable>
using namespace std;

#include "io/PatternsCollector.hpp"
#include "util/tclap/CmdLine.h"
#include "util/ReadWriteLock.hpp"

using io::PatternsCollector;
using util::ReadWriteLock;

class PLCMThread;

namespace util {
class ProgressWatcherThread;
}

namespace internals {
class ExplorationStep;
}

using internals::ExplorationStep;
using util::ProgressWatcherThread;

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

	map<PLCMCounters, const string> PLCMCountersNames = {
			{ ExplorationStepInstances, "ExplorationStepInstances" },
			{ ExplorationStepCatchedWrongFirstParents, "ExplorationStepCatchedWrongFirstParents" },
			{ FirstParentTestRejections, "FirstParentTestRejections" },
			{ TransactionsCompressions, "TransactionsCompressions" }};

protected:
	struct Options {
		bool benchmark_mode;
		bool memory_usage;
		bool sort_items;
		uint32_t num_threads;
		bool verbose;
		bool ultra_verbose;
		string input_path;
		uint32_t minsup;
		string output_path;
		bool output_all;
	};

	typedef map<thread::id, unique_ptr<PLCMThread> > 	thread_map;
    static unique_ptr<thread_map> threads;
    static thread_map *threads_fast;

private:
    unique_ptr<ProgressWatcherThread> progressWatch;
    unique_ptr<PatternsCollector> collector;
    int64_t globalCounters[PLCMCounters::Number_of_PLCMCounters];

public:
    PLCM(struct Options *options);
    ~PLCM();

    void collect(int32_t support, vector<int32_t>* pattern);
    void lcm(shared_ptr<ExplorationStep> initState, uint32_t num_threads);
    void display(ostream& stream, map<string, uint64_t>& additionalCounters);
    int64_t closeCollector();
    int32_t getAveragePatternLength();
    int32_t getMaxPatternLength();

    static int main(int argc, char**argv);
    static void standalone(unique_ptr<PLCM::Options> options);
    static PLCMThread* getCurrentThread();
    shared_ptr<ExplorationStep> stealJob(PLCMThread* thief);

protected:
    void createThreads(int32_t nbThreads);
    void initializeAndStartThreads(shared_ptr<ExplorationStep> initState);

private:
    static uint32_t getDefaultNumThreads();
    static unique_ptr<PatternsCollector> instanciateCollector(
    		struct Options* options);
};

class PLCMThread
{
protected:
	ReadWriteLock _lock;
	unique_ptr<thread> _thread_storage;
	thread *_thread;
    unique_ptr<vector<shared_ptr<ExplorationStep> > > stackedJobs_storage;
    vector<shared_ptr<ExplorationStep> > *stackedJobs;
    PLCM *_PLCM_instance;
    mutex starting_mutex;
    condition_variable cond_should_start;
    bool should_start;
    int _index_cpu;
    uint32_t _human_readable_id;

public:
    uint64_t counters[PLCM::PLCMCounters::Number_of_PLCMCounters];

public:
    PLCMThread(uint32_t human_readable_id,
    		PLCM *PLCM_instance, int index_cpu);
    thread::id getId();
    uint32_t getHumanReadableId();
    void run();
    void join();
    void init(shared_ptr<ExplorationStep> initState);
    void start();
    shared_ptr<ExplorationStep> giveJob(PLCMThread *thief);

private:
    void lcm(shared_ptr<ExplorationStep> state);
};

