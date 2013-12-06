#include <io/FileCollector.hpp>

#include <iostream>
using namespace std;

bool fileExists(string &fileName)
{
    ifstream infile(fileName);
    return infile.good();
}

namespace io {

FileCollector::FileCollector(string& path) {

	if (fileExists(path)) {
		cerr << "Warning : overwriting output file " << path << endl;
	}

	collected = 0;
	collectedLength = 0;
	stream = new ofstream(path);
}

FileCollector::~FileCollector() {
	delete stream;
}

void FileCollector::collect(int32_t support, vector<uint32_t>* pattern) {
	(*stream) << support << "\t";

	bool addSeparator = false;
	for (int item : (*pattern)) {
		if (addSeparator) {
			(*stream) << ' ';
		} else {
			addSeparator = true;
		}

		(*stream) << item;
	}

	(*stream) << endl;
	collected++;
	collectedLength += pattern->size();
}

int64_t FileCollector::close() {
	stream->flush();
	stream->close();
	return collected;
}

int32_t FileCollector::getAveragePatternLength() {
	if (collected == 0) {
		return 0;
	} else {
		return (collectedLength / collected);
	}
}

int64_t FileCollector::getCollected() {
	return collected;
}

int64_t FileCollector::getCollectedLength() {
	return collectedLength;
}

}
