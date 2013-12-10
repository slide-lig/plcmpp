
#include <io/NullCollector.hpp>

io::NullCollector::NullCollector() {
	collectedCount = 0;
	collectedLength = 0;
}

void io::NullCollector::collect(int32_t support, vector<int32_t>* pattern) {
	collectedCount += 1;
	collectedLength += pattern->size();
}

int64_t io::NullCollector::close() {
	return collectedCount.load();
}

int32_t io::NullCollector::getAveragePatternLength() {
	if (collectedCount.load() == 0) {
		return 0;
	} else {
		return (collectedLength.load() / collectedCount.load());
	}
}
