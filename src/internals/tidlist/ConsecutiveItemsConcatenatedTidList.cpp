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
#include <iostream>
using namespace std;

#include <internals/tidlist/ConsecutiveItemsConcatenatedTidList.hpp>
#include <internals/Counters.hpp>

using util::array_int32;

namespace internals {
namespace tidlist {

ConsecutiveItemsConcatenatedTidList::ConsecutiveItemsConcatenatedTidList(
		Counters* c, int32_t highestTidList) :
		ConsecutiveItemsConcatenatedTidList(
				c->distinctTransactionsCounts.get(), highestTidList) {
}

ConsecutiveItemsConcatenatedTidList::ConsecutiveItemsConcatenatedTidList(
		p_array_int32 lengths, int32_t highestTidList) {
	int32_t startPos = 0;
	int32_t top = min(highestTidList, (int32_t)lengths->size());
	auto lengths_fast = lengths->array;
	_indexAndFreqs = new array_int32(top * 2, 0);
	_indexAndFreqs_fast = _indexAndFreqs->array;
	_indexAndFreqs_size = _indexAndFreqs->size();
	auto end = _indexAndFreqs->end();
	int32_t *itemIndex;
	for (itemIndex = _indexAndFreqs_fast;
			itemIndex < end; itemIndex+=2, ++lengths_fast)
	{
		if (*lengths_fast > 0) {
			*itemIndex = startPos;
			startPos += *lengths_fast;
		} else {
			*itemIndex = -1;
		}
	}
	_storage_size = startPos;
}

ConsecutiveItemsConcatenatedTidList::ConsecutiveItemsConcatenatedTidList(
		const ConsecutiveItemsConcatenatedTidList& other) {
	_indexAndFreqs = new array_int32(*(other._indexAndFreqs));
	_indexAndFreqs_fast = _indexAndFreqs->array;
	_indexAndFreqs_size = _indexAndFreqs->size();
	_storage_size = other._storage_size;
}

ConsecutiveItemsConcatenatedTidList::~ConsecutiveItemsConcatenatedTidList() {
	delete _indexAndFreqs;
}

void ConsecutiveItemsConcatenatedTidList::addTransaction(
		int32_t item, int32_t transaction) {
	uint32_t itemIndex = item << 1;
	if (itemIndex > _indexAndFreqs_size ||
			_indexAndFreqs_fast[itemIndex] == -1) {
		cerr << "item " << item << " has no tidlist" << endl;
		abort();
	}
	int32_t start = _indexAndFreqs_fast[itemIndex];
	int32_t index = _indexAndFreqs_fast[itemIndex + 1];
	write(start + index, transaction);
	_indexAndFreqs_fast[itemIndex + 1]++;
}

void ConsecutiveItemsConcatenatedTidList::resetTidLists()
{
	auto start = _indexAndFreqs_fast +1;
	auto end = start + _indexAndFreqs_size;

	for (auto it = start; it != end; it += 2)
	{
		*it = 0;
	}
}

}
}
