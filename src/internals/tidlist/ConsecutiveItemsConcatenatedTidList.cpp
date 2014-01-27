
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
	for (int32_t i = 0; i < top; i++) {
		int32_t itemIndex = i << 1;
		if (lengths_fast[i] > 0) {
			_indexAndFreqs_fast[itemIndex] = startPos;
			startPos += lengths_fast[i];
		} else {
			_indexAndFreqs_fast[itemIndex] = -1;
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

unique_ptr<Iterator<int32_t> > ConsecutiveItemsConcatenatedTidList::get(
		int32_t item) {
	uint32_t itemIndex = item << 1;
	if (itemIndex > _indexAndFreqs_size ||
			_indexAndFreqs_fast[itemIndex] == -1) {
		cerr << "item " << item << " has no tidlist" << endl;
		abort();
	}
	int32_t startPos = _indexAndFreqs_fast[itemIndex];
	int32_t length = _indexAndFreqs_fast[itemIndex + 1];
	return unique_ptr<Iterator<int32_t> >(
			new TidIterator(this, length, startPos));
}

unique_ptr<TidList::TIntIterable>
		ConsecutiveItemsConcatenatedTidList::getIterable(
		int32_t item) {
	return unique_ptr<TidList::TIntIterable>(
			new TidIterable(this, item));
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

TidIterable::TidIterable(
		ConsecutiveItemsConcatenatedTidList* tidlist, int32_t item) {
	_tidlist = tidlist;
	_item = item;
}

unique_ptr<Iterator<int32_t> >
	TidIterable::iterator() {
		return _tidlist->get(_item);
}

TidIterator::TidIterator(
		ConsecutiveItemsConcatenatedTidList* tidlist,
		int32_t length,
		int32_t startPos) {
	_tidlist = tidlist;
	_length = length;
	_startPos = startPos;
	_index = 0;
}

bool TidIterator::hasNext() {
	return _index < _length;
}

int32_t TidIterator::next() {
	return _tidlist->read(_startPos + (_index++));
}

}
}
