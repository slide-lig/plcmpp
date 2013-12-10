
#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;

#include <io/StdOutCollector.hpp>


io::StdOutCollector::StdOutCollector() {
	collected = 0;
	collectedLength = 0;
}

void io::StdOutCollector::collect(int32_t support, vector<int32_t>* pattern) {
	// print...
	cout << support << "\t[ ";

	copy(pattern->begin(), pattern->end()-1,
	              ostream_iterator<int32_t>(cout, ", "));

	cout << *(pattern->end()) << " ]" << endl;

	collected++;
	collectedLength += pattern->size();
}

int64_t io::StdOutCollector::close() {
	return collected;
}

int32_t io::StdOutCollector::getAveragePatternLength() {
	if (collected == 0) {
		return 0;
	} else {
		return collectedLength / collected;
	}
}
