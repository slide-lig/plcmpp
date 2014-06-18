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

#include <algorithm>

#include <io/PatternSortCollector.hpp>

io::PatternSortCollector::PatternSortCollector(PatternsCollector* wrapped) {
	decorated = wrapped;
}

void io::PatternSortCollector::collect(int32_t support,
		vector<int32_t>* pattern) {
	std::sort(pattern->begin(), pattern->end());
	decorated->collect(support, pattern);
}

int64_t io::PatternSortCollector::close() {
	return decorated->close();
}

int32_t io::PatternSortCollector::getAveragePatternLength() {
	return decorated->getAveragePatternLength();
}
