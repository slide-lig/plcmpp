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
	collectedMaxLength = 0;
	stream = new ofstream(path);
}

FileCollector::~FileCollector() {
	delete stream;
}

void FileCollector::collect(int32_t support, vector<int32_t>* pattern) {
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
	if(pattern->size() > collectedMaxLength){
		collectedMaxLength = pattern->size();
	}
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

vector<int32_t>::size_type FileCollector::getMaxPatternLength() {
	return collectedMaxLength;
}
}
