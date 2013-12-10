
#include <sstream>

#include <io/MultiThreadedFileCollector.hpp>
#include <io/FileCollector.hpp>

namespace io {

MultiThreadedFileCollector::MultiThreadedFileCollector(string& prefix) {
	collectors = new map<thread::id, FileCollector*>();
	_prefix = prefix;
}

MultiThreadedFileCollector::~MultiThreadedFileCollector() {
	for (pair<const std::thread::id, io::FileCollector*> pair : (*collectors)) {
			delete pair.second;
	}

	delete collectors;
}

FileCollector *MultiThreadedFileCollector::getCollectorOfCurrentThread() {
	thread::id this_thread_id = std::this_thread::get_id();
	typename map<thread::id, FileCollector*>::iterator it =
					collectors->find(this_thread_id);
	FileCollector *result;
	if (it == collectors->end())
	{	// not found, create the collector

		std::ostringstream spath(_prefix);
		spath << this_thread_id << ".dat";
		string path = spath.str();
		result = new FileCollector(path);
		(*collectors)[this_thread_id] = result;
	}
	else
	{	// found
		result = it->second;
	}

	return result;
}

void MultiThreadedFileCollector::collect(int32_t support,
		vector<int32_t>* pattern) {
	getCollectorOfCurrentThread()->collect(support, pattern);
}

int64_t MultiThreadedFileCollector::close() {
	int64_t total = 0;

	for (pair<const std::thread::id, io::FileCollector*> pair : (*collectors)) {
		total += pair.second->close();
	}

	return total;
}

int32_t MultiThreadedFileCollector::getAveragePatternLength() {
	int64_t totalLen = 0;
	int64_t nbPatterns = 0;

	for (pair<const std::thread::id, io::FileCollector*> pair : (*collectors)) {
		totalLen += pair.second->getCollectedLength();
		nbPatterns += pair.second->getCollected();
	}

	return (int) (totalLen / nbPatterns);
}

}
