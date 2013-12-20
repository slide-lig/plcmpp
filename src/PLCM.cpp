
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
#include <util/MemoryPeakWatcherThread.hpp>
#include <util/ProgressWatcherThread.hpp>
#include <util/Helpers.h>

using namespace util;
using namespace io;

unique_ptr<PLCM::thread_map> PLCM::threads = nullptr;

PLCM::PLCM(struct Options *options) {
	uint32_t nbThreads = options->num_threads;
	if (nbThreads < 1) {
		cerr << "nbThreads has to be > 0, given " << nbThreads <<
				". Aborting.";
		exit(EXIT_FAILURE);
	}
	collector = instanciateCollector(options);
	threads = unique_ptr<thread_map>(new thread_map());
	createThreads(nbThreads);
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

void PLCM::lcm(shared_ptr<ExplorationStep> initState) {
	if (initState->pattern->size() > 0) {
		collector->collect(
				initState->counters->transactionsCount,
				initState->pattern.get());
	}

	progressWatch->setInitState(initState);

	initializeAndStartThreads(initState);

	progressWatch->start();

	for (auto it = threads->begin(); it != threads->end(); ++it) {
		auto t = it->second.get();
		t->join();
		for (uint i = 0; i < PLCMCounters::Number_of_PLCMCounters; ++i) {
			globalCounters[i] += t->counters[i];
		}
	}

	progressWatch->stop();
}

void PLCM::display(ostream& stream,
		map<string, uint64_t>* additionalcounters) {
	stream << "{\"name\":\"PLCM\", \"threads\":" << threads->size();

	for (uint i = 0; i < PLCMCounters::Number_of_PLCMCounters; ++i) {
		stream << ", \"" << PLCMCountersNames[(PLCMCounters)i] <<
				"\":" << globalCounters[i];
	}

	if (additionalcounters != nullptr) {
		for (auto entry : (*additionalcounters)) {
			stream << ", \"" << entry.first << "\":" << entry.second;
		}
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
			"Give peak memory usage after mining "
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
			output_path.getValue()
	});

	// run the main procedure
	standalone(move(options) /* transfer ownership */);
	return 0;
}

void PLCM::standalone(unique_ptr<PLCM::Options> options) {
	unique_ptr<MemoryPeakWatcherThread> memoryWatch = nullptr;

	if (options->memory_usage) {
		memoryWatch = unique_ptr<MemoryPeakWatcherThread>(
				new MemoryPeakWatcherThread());
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

	double chrono = Helpers::precise_time();
	shared_ptr<ExplorationStep> initState = make_shared<ExplorationStep>(
			options->minsup, options->input_path);
	double loadingTime = Helpers::precise_time() - chrono;
	cerr << "Dataset loaded in " << loadingTime << "s" << endl;

	unique_ptr<PLCM> miner(new PLCM(options.get()));

	chrono = Helpers::precise_time();
	miner->lcm(initState);
	chrono = Helpers::precise_time() - chrono;

	unique_ptr<map<string, uint64_t> > additionalcounters(
			new map<string, uint64_t>());
	(*additionalcounters)["miningTime"] = chrono*1000 /* milliseconds */;
	(*additionalcounters)["outputtedPatterns"] = miner->closeCollector();
	(*additionalcounters)["loadingTime"] = loadingTime*1000 /* milliseconds */;
	(*additionalcounters)["avgPatternLength"] =
			(uint64_t) miner->getAveragePatternLength();

	if (memoryWatch != nullptr) {
		memoryWatch->stop();
		(*additionalcounters)["maxUsedMemory"] = memoryWatch->getMaxUsedMemory();
	}

	miner->display(cerr, additionalcounters.get());
}

PLCMThread* PLCM::getCurrentThread() {
	return (*threads)[this_thread::get_id()].get();
}

void PLCM::createThreads(int32_t nbThreads) {
	for (int i = 0; i < nbThreads; i++) {
		auto t = new PLCMThread(this);
		(*threads)[t->getId()] = unique_ptr<PLCMThread>(t);
	}
}

void PLCM::initializeAndStartThreads(shared_ptr<ExplorationStep> initState) {
	for (auto it = threads->begin(); it != threads->end(); ++it) {
		it->second->init(initState);
		it->second->start();
	}
}

shared_ptr<ExplorationStep> PLCM::stealJob(PLCMThread* thief) {
	for (auto it = threads->begin(); it != threads->end(); ++it) {
		auto victim = it->second.get();
		if (victim != thief) {
			auto e = victim->giveJob(thief);
			if (e != nullptr) {
				return e;
			}
		}
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

PLCMThread::PLCMThread(PLCM* PLCM_instance) {
	stackedJobs = unique_ptr<deque<shared_ptr<ExplorationStep> > >(
			new deque<shared_ptr<ExplorationStep> >());
	_PLCM_instance = PLCM_instance;
	should_start = false; // wait for the signal
	_thread = unique_ptr<thread>(new thread(&PLCMThread::run, this));
	for (uint i = 0; i < PLCM::PLCMCounters::Number_of_PLCMCounters; ++i) {
		counters[i] = 0;
	}
}

thread::id PLCMThread::getId() {
	return _thread->get_id();
}

void PLCMThread::run() {
	// 1st, wait for the start condition
	{
		unique_lock<mutex> ul(_mutex);
		while (!should_start)	// handle spurious wake-ups
		{
			cond_should_start.wait(ul);
		}
	}

	// no need to readlock, this thread is the only one that can do
	// writes
	bool exit = false;
	while (!exit) {
		ExplorationStep *sj = nullptr;
		if (!stackedJobs->empty()) {
			sj = stackedJobs->back().get();

			unique_ptr<ExplorationStep> extended = sj->next();
			// iterator is finished, remove it from the stack
			if (extended == nullptr) {
				lock_guard<mutex> lg(_mutex);
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
	lock_guard<mutex> lg(_mutex);
	stackedJobs->push_back(initState);
}

void PLCMThread::join() {
	_thread->join();
}

void PLCMThread::start() {
	{
		lock_guard<mutex> lg(_mutex);
		should_start = true;
	}
	cond_should_start.notify_one();
}

void PLCMThread::lcm(shared_ptr<ExplorationStep> state) {
	_PLCM_instance->collect(
			state->counters->transactionsCount,
			state->pattern.get());

	lock_guard<mutex> lg(_mutex);
	stackedJobs->push_back(state);
}

shared_ptr<ExplorationStep> PLCMThread::giveJob(PLCMThread* thief) {
	// here we need to readlock because the owner thread can write
	lock_guard<mutex> lg(_mutex);
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


