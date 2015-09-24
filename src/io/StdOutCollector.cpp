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

#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;

#include <io/StdOutCollector.hpp>
#include <util/Helpers.h>
using util::Helpers;

io::StdOutCollector::StdOutCollector() {
	collected = 0;
	collectedLength = 0;
	collectedMaxLength = 0;
}

void io::StdOutCollector::collect(int32_t support, vector<int32_t>* pattern) {
	// print...
	cout << support << "\t" <<
			Helpers::printed_vector(pattern) << endl;

	collected++;
	collectedLength += pattern->size();
	if(pattern->size() > collectedMaxLength){
		collectedMaxLength = pattern->size();
	}
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

vector<int32_t>::size_type io::StdOutCollector::getMaxPatternLength() {
	return collectedMaxLength;
}
