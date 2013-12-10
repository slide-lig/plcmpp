
#include <algorithm>

#include <io/PatternSortCollector.hpp>

io::PatternSortCollector::PatternSortCollector(PatternsCollector* wrapped) {
	decorated = wrapped;
}

void io::PatternSortCollector::collect(int32_t support,
		vector<int32_t>* pattern) {

	// TODO: check if a thread_local static allocation would perform better.
	vector<int32_t> sorted(pattern->size());
	std::copy(pattern->begin(), pattern->end(), std::back_inserter(sorted));
	std::sort(sorted.begin(), sorted.end());
	decorated->collect(support, &sorted);
}

int64_t io::PatternSortCollector::close() {
	return decorated->close();
}

int32_t io::PatternSortCollector::getAveragePatternLength() {
	return decorated->getAveragePatternLength();
}
