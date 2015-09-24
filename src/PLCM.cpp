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

#include <sstream>
using namespace std;

#include <PLCM.hpp>

#include <internals/Counters.hpp>
#include <internals/Dataset.hpp>
#include <internals/ExplorationStep.hpp>
#include <io/MultiThreadedFileCollector.hpp>
#include <io/NullCollector.hpp>
#include <io/PatternSortCollector.hpp>
#include <io/StdOutCollector.hpp>
#include <util/MemoryUsage.hpp>
#include <util/ProgressWatcherThread.hpp>
#include <util/Helpers.h>
#include <util/SimpleDigest.h>

using namespace util;
using namespace io;

// initialize static variables
unique_ptr<PLCM::thread_map> PLCM::threads = nullptr;
PLCM::thread_map* PLCM::threads_fast = nullptr;

PLCM::PLCM(struct Options *options) {
	uint32_t nbThreads = options->num_threads;
	if (nbThreads < 1) {
		cerr << "nbThreads has to be > 0, given " << nbThreads <<
				". Aborting.";
		exit(EXIT_FAILURE);
	}
	collector = instanciateCollector(options);
	threads = unique_ptr<thread_map>(new thread_map());
	threads_fast = threads.get();
	progressWatch = unique_ptr<ProgressWatcherThread>(
			new ProgressWatcherThread());
	for (uint i = 0; i < PLCMCounters::Number_of_PLCMCounters; ++i) {
				globalCounters[i] = 0;
	}
}

PLCM::~PLCM() {
}

void PLCM::collect(int32_t support, vector<int32_t>* pattern) {
	collector->collect(support, pattern);
}

void PLCM::lcm(shared_ptr<ExplorationStep> initState, uint32_t num_threads) {
	if (initState->pattern->size() > 0) {
		collector->collect(
				initState->counters->transactionsCount,
				initState->pattern.get());
	}

	progressWatch->setInitState(initState);

	createThreads(num_threads);
	initializeAndStartThreads(initState);

	progressWatch->start();

	for (auto it = threads_fast->begin(); it != threads_fast->end(); ++it) {
		auto t = it->second.get();
		t->join();
		for (uint i = 0; i < PLCMCounters::Number_of_PLCMCounters; ++i) {
			globalCounters[i] += t->counters[i];
		}
	}

	progressWatch->stop();
}

void PLCM::display(ostream& stream,
		map<string, uint64_t>& additionalcounters) {
	stream << "{\"name\":\"PLCM\", \"threads\":" << threads_fast->size();

	for (uint i = 0; i < PLCMCounters::Number_of_PLCMCounters; ++i) {
		stream << ", \"" << PLCMCountersNames[(PLCMCounters)i] <<
				"\":" << globalCounters[i];
	}

	for (auto entry : additionalcounters) {
		stream << ", \"" << entry.first << "\":" << entry.second;
	}

	stream << "}" << endl;
}

int64_t PLCM::closeCollector() {
	return collector->close();
}

int32_t PLCM::getAveragePatternLength() {
	return collector->getAveragePatternLength();
}

uint32_t PLCM::getDefaultNumThreads() {
	unsigned concurentThreadsSupported = thread::hardware_concurrency();
	if (concurentThreadsSupported == 0)
	{
		cerr << "Warning: could not get the number of concurrent threads supported"
				" by your system." << endl;
		concurentThreadsSupported = 4;
	}
	else
	{
	    cout << "Info: your system supports " << concurentThreadsSupported <<
	    		" concurrent threads." << endl;
	}

	return concurentThreadsSupported;
}

int PLCM::main(int argc, char** argv) {

	uint32_t default_num_threads = getDefaultNumThreads();

	TCLAP::CmdLine cmd("Parallel LCM, C++ implementation.");

	TCLAP::SwitchArg benchmark_mode("b","benchmark",
			"Benchmark mode: patterns are not outputted at "
			"all (in which case OUTPUT_PATH is ignored)", cmd);
	TCLAP::SwitchArg memory_usage("m","mem-usage",
			"Periodically display memory usage during execution "
			"(instanciates a periodic watcher thread)", cmd);
	TCLAP::SwitchArg sort_items("s", "sort-items",
			"Sort items in outputted patterns, in ascending order", cmd);
	ostringstream oss_explain;
	oss_explain << "How many threads will be launched (defaults to " <<
		default_num_threads << ")";
	TCLAP::ValueArg<uint32_t> num_threads("t", "num-threads",
			oss_explain.str(), false, default_num_threads,
			"num_threads", cmd);
	TCLAP::SwitchArg verbose("v", "verbose",
			"Enable verbose mode, which logs every extension "
			"of the empty pattern", cmd);
	TCLAP::SwitchArg ultra_verbose("V", "ultra-verbose",
			"Enable ultra-verbose mode, which logs every pattern extension"
			" (use with care: it may produce a LOT of output)", cmd);
	TCLAP::SwitchArg output_closed_only("c", "closed-patterns",
			"Enable generating closed patterns only"
			" Limits the number of results generated by reducing redundancy", cmd);
	TCLAP::UnlabeledValueArg<string> input_path("INPUT_PATH",
			"The path of the file to mine into.", true, "None", "in_file", cmd);
	TCLAP::UnlabeledValueArg<uint32_t> minsup("MINSUP",
			"The minimum support of items.", true, 10, "min_support", cmd);
	TCLAP::UnlabeledValueArg<string> output_path("OUTPUT_PATH",
			"The path of the file where results should be dumped "
			"(defaults to standard output).", false, "-", "out_file", cmd);

	// Parse the argv array	(will exit if invalid)
	cmd.parse( argc, argv );

	unique_ptr<PLCM::Options> options(new PLCM::Options {
			benchmark_mode.getValue(),
			memory_usage.getValue(),
			sort_items.getValue(),
			num_threads.getValue(),
			verbose.getValue(),
			ultra_verbose.getValue(),
			input_path.getValue(),
			minsup.getValue(),
			output_path.getValue(),
			output_closed_only.getValue()
	});

	// run the main procedure
	standalone(move(options) /* transfer ownership */);

	REPORT_DIGEST_STATS();
	return 0;
}

void PLCM::standalone(unique_ptr<PLCM::Options> options) {
	unique_ptr<MemoryUsage::WatcherThread> memoryWatch = nullptr;

	if (options->memory_usage) {
		memoryWatch = unique_ptr<MemoryUsage::WatcherThread>(
				new MemoryUsage::WatcherThread());
		memoryWatch->start();
	}

	if (options->ultra_verbose)
	{
		ExplorationStep::verbose = true;
		ExplorationStep::ultraVerbose = true;
	}
	else
	{
		ExplorationStep::verbose = options->verbose;
	}

	if (options->output_all)
	{
		ExplorationStep::outputClosedPatternsOnly = true;
	}
	else
	{
		ExplorationStep::outputClosedPatternsOnly = false;
	}

	double chrono = Helpers::precise_time();
	shared_ptr<ExplorationStep> initState = make_shared<ExplorationStep>(
			options->minsup, options->input_path);
	double loadingTime = Helpers::precise_time() - chrono;
	cerr << "Dataset loaded in " << loadingTime << "s" << endl;

	PLCM miner(options.get());

	chrono = Helpers::precise_time();
	miner.lcm(initState, options->num_threads);
	chrono = Helpers::precise_time() - chrono;

	map<string, uint64_t> additionalcounters;
	additionalcounters["miningTime"] = chrono*1000 /* milliseconds */;
	additionalcounters["outputtedPatterns"] = miner.closeCollector();
	additionalcounters["loadingTime"] = loadingTime*1000 /* milliseconds */;
	additionalcounters["avgPatternLength"] =
			(uint64_t) miner.getAveragePatternLength();

	if (memoryWatch != nullptr) {
		memoryWatch->stop();
	}

	/* We always give this because its cheap: the kernel records
	 * the peak memory usage accross the life of processes,
	 * so we just have to request it here, at the end of the execution. */
	additionalcounters["maxUsedMemory"] = MemoryUsage::getPeakMemoryUsage();

	miner.display(cerr, additionalcounters);
}

PLCMThread* PLCM::getCurrentThread() {
	static __thread PLCMThread *result = nullptr;

	if (result == nullptr)
		result = (*threads_fast)[this_thread::get_id()].get();
	return result;
}

void PLCM::createThreads(int32_t nbThreads) {
	cpu_set_t cpu_set;
	sched_getaffinity(0, sizeof(cpu_set_t), &cpu_set);
	int num_cpus = CPU_COUNT(&cpu_set);
	int index_cpu;

	for (int i = 0; i < nbThreads; i++) {
		index_cpu = i%num_cpus;
		auto t = new PLCMThread(i, this, index_cpu);
		(*threads_fast)[t->getId()] = unique_ptr<PLCMThread>(t);
	}
}

void PLCM::initializeAndStartThreads(shared_ptr<ExplorationStep> initState) {
	for (auto it = threads_fast->begin(); it != threads_fast->end(); ++it) {
		it->second->init(initState);
		it->second->start();
	}
}

shared_ptr<ExplorationStep> PLCM::stealJob(PLCMThread* thief) {

	/* we try to avoid too much concurrency, with all thread trying
	 * to steal jobs to the same victim.
	 *
	 * For example, thread 3 will preferably steal a job to thread 4,
	 * or 5, or 6, ... or N, or 0, or 1, or 2.
	 */

	const thread::id thief_id = thief->getId();

	// point to the thread following the thief
	auto it = threads_fast->begin();
	for (; it->first != thief_id; ++it) { }
	++it;

	// the thief can steal a job to any thread except itself
	size_t num_stealable_threads = threads_fast->size() -1;

	shared_ptr<ExplorationStep> job;
	// try each thread pointed by it, in turn
	for (size_t index = 0; index < num_stealable_threads; ++index) {
		if (it == threads_fast->end())
			it = threads_fast->begin();
		job = it->second->giveJob(thief);
		if (job != nullptr) {
			//cout << "Job stolen: thread " << it->second->getHumanReadableId()
			//		<< " to " << thief->getHumanReadableId() << endl;
			return job;
		}

		++it;
	}
	return nullptr;
}

unique_ptr<PatternsCollector> PLCM::instanciateCollector(PLCM::Options *options) {
	PatternsCollector* collector = nullptr;

	if (options->benchmark_mode) {
		collector = new NullCollector();
	} else {
		if (options->output_path != "-") {
			collector = new MultiThreadedFileCollector(
						options->output_path);
		} else {
			collector = new StdOutCollector();
		}

		if (options->sort_items) {
			collector = new PatternSortCollector(collector);
		}
	}

	return unique_ptr<PatternsCollector>(collector);
}

PLCMThread::PLCMThread(uint32_t human_readable_id,
		PLCM* PLCM_instance, int index_cpu) {
	_human_readable_id = human_readable_id;
	stackedJobs = new vector<shared_ptr<ExplorationStep> >();
	stackedJobs_storage = unique_ptr<vector<shared_ptr<ExplorationStep> > >(
			stackedJobs); // auto delete

	_PLCM_instance = PLCM_instance;
	_index_cpu = index_cpu;
	should_start = false; // wait for the signal
	_thread = new thread(&PLCMThread::run, this);
	_thread_storage = unique_ptr<thread>(_thread); // auto delete
	for (uint i = 0; i < PLCM::PLCMCounters::Number_of_PLCMCounters; ++i) {
		counters[i] = 0;
	}
}

thread::id PLCMThread::getId() {
	return _thread->get_id();
}

uint32_t PLCMThread::getHumanReadableId() {
	return _human_readable_id;
}

void PLCMThread::run() {
	// 1st, set the thread affinity
	cpu_set_t cpu_set;
	CPU_ZERO(&cpu_set);
	CPU_SET(_index_cpu, &cpu_set);
	sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set);

	// then, wait for the start condition
	{
		unique_lock<mutex> ul(starting_mutex);
		while (!should_start)	// handle spurious wake-ups
		{
			cond_should_start.wait(ul);
		}
	}

	// no need to readlock, this thread is the only one that can do
	// writes
	bool exit = false;
	while (!exit) {
		shared_ptr<ExplorationStep> sj = nullptr;
		if (!stackedJobs->empty()) {
			sj = stackedJobs->back();

			unique_ptr<ExplorationStep> extended = sj->next();
			// iterator is finished, remove it from the stack
			if (extended == nullptr) {
				ReadWriteLock::WriteGuard lg(_lock);
				stackedJobs->pop_back();
				counters[PLCM::PLCMCounters::ExplorationStepInstances]++;
				counters[PLCM::PLCMCounters::ExplorationStepCatchedWrongFirstParents] +=
						sj->getCatchedWrongFirstParentCount();
			} else {
				lcm(move(extended) /* transfer ownership */);
			}
		} else { // our list was empty, we should steal from another
					// thread
			shared_ptr<ExplorationStep> stolj = _PLCM_instance->stealJob(this);
			if (stolj == nullptr) {
				exit = true;
			} else {
				lcm(stolj);
			}
		}
	}
}

void PLCMThread::init(shared_ptr<ExplorationStep> initState) {
	ReadWriteLock::WriteGuard lg(_lock);
	stackedJobs->push_back(initState);
}

void PLCMThread::join() {
	_thread->join();
}

void PLCMThread::start() {
	{
		lock_guard<mutex> lg(starting_mutex);
		should_start = true;
	}
	cond_should_start.notify_one();
}

void PLCMThread::lcm(shared_ptr<ExplorationStep> state) {
#ifdef PRINT_STEPS
	cout << state->id << endl;
#endif
	_PLCM_instance->collect(
			state->counters->transactionsCount,
			state->pattern.get());
	{
		ReadWriteLock::WriteGuard lg(_lock);
		stackedJobs->push_back(state);
	}
}

shared_ptr<ExplorationStep> PLCMThread::giveJob(PLCMThread* thief) {
	// here we need to readlock because the owner thread can write
	ReadWriteLock::ReadGuard lg(_lock);
	for (uint32_t stealPos = 0; stealPos < stackedJobs->size(); stealPos++) {
		shared_ptr<ExplorationStep> sj = stackedJobs->at(stealPos);
		shared_ptr<ExplorationStep> next = Helpers::unique_to_shared(sj->next());

		if (next != nullptr) {
			thief->init(sj);
			return next;
		}
	}
	return nullptr;
}


