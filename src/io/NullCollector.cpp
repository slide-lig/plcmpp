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

#include <io/NullCollector.hpp>

io::NullCollector::NullCollector() {
	collectedCount = 0;
	collectedLength = 0;
	collectedMaxLength = 0;
}

void io::NullCollector::collect(int32_t support, vector<int32_t>* pattern) {
	collectedCount += 1;
	collectedLength += pattern->size();
	vector<int32_t>::size_type currentMax = collectedMaxLength.load();
	while(pattern->size() > currentMax && collectedMaxLength.compare_exchange_strong(currentMax, pattern->size())){
	}
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

vector<int32_t>::size_type io::NullCollector::getMaxPatternLength() {
	return collectedMaxLength.load();
}
