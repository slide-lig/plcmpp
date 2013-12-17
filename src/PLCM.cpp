
#include <PLCM.hpp>

#include <internals/Counters.hpp>
#include <io/MultiThreadedFileCollector.hpp>
#include <io/NullCollector.hpp>
#include <io/PatternSortCollector.hpp>
#include <io/StdOutCollector.hpp>
#include <util/MemoryPeakWatcherThread.hpp>
#include <util/ProgressWatcherThread.hpp>

PLCM::PLCM(PatternsCollector* patternsCollector, int32_t nbThreads) {
}

PLCM::~PLCM() {
}

void PLCM::collect(int32_t support, vector<int32_t>* pattern) {
}

void PLCM::lcm(ExplorationStep* initState) {
}

unique_ptr<map<PLCM::PLCMCounters, uint64_t> > PLCM::getCounters() {
}

void PLCM::display(ostream& stream,
		map<PLCMCounters, uint64_t>* additionalCounters) {
}

int PLCM::main(int argc, char** argv) {
}

void PLCM::printMan() {
}

void PLCM::standalone(TCLAP::CmdLine& cmd) {
}

PLCMThread* PLCM::getCurrentThread() {
}

void PLCM::createThreads(int32_t nbThreads) {
}

void PLCM::initializeAndStartThreads(ExplorationStep* initState) {
}

ExplorationStep* PLCM::stealJob(PLCMThread* thief) {
}

ExplorationStep* PLCM::stealJob(PLCMThread* thief, PLCMThread* victim) {
}

unique_ptr<PatternsCollector> PLCM::instanciateCollector(TCLAP::CmdLine& cmd,
		string& outputPath, int32_t nbThreads) {
}

PLCMThread::PLCMThread(PLCM* PLCM_instance, int32_t id) {
}

thread::id PLCMThread::getId() {
}

void PLCMThread::run() {
}

void PLCMThread::init(ExplorationStep* initState) {
}

void PLCMThread::lcm(ExplorationStep* state) {
}
