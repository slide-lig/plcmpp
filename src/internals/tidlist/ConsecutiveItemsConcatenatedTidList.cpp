
#include <algorithm>
#include <iostream>
using namespace std;

#include <internals/tidlist/ConsecutiveItemsConcatenatedTidList.hpp>
#include <internals/Counters.hpp>

namespace internals {
namespace tidlist {

ConsecutiveItemsConcatenatedTidList::ConsecutiveItemsConcatenatedTidList(
		Counters* c, int32_t highestTidList) :
		ConsecutiveItemsConcatenatedTidList(
				c->distinctTransactionsCounts, highestTidList) {
}

ConsecutiveItemsConcatenatedTidList::ConsecutiveItemsConcatenatedTidList(
		vector<int32_t>* lengths, int32_t highestTidList) {
	int32_t startPos = 0;
	int32_t top = min(highestTidList, (int32_t)lengths->size());
	_indexAndFreqs = new vector<int32_t>(top * 2);
	for (int32_t i = 0; i < top; i++) {
		int32_t itemIndex = i << 1;
		if ((*lengths)[i] > 0) {
			(*_indexAndFreqs)[itemIndex] = startPos;
			startPos += (*lengths)[i];
		} else {
			(*_indexAndFreqs)[itemIndex] = -1;
		}
	}
	_storage_size = startPos;
}

ConsecutiveItemsConcatenatedTidList::ConsecutiveItemsConcatenatedTidList(
		const ConsecutiveItemsConcatenatedTidList& other) {
	_indexAndFreqs = new vector<int32_t>(*(other._indexAndFreqs));
	_storage_size = other._storage_size;
}

unique_ptr<Iterator<int32_t> > ConsecutiveItemsConcatenatedTidList::get(
		int32_t item) {
	uint32_t itemIndex = item << 1;
	if (itemIndex > _indexAndFreqs->size() ||
			(*_indexAndFreqs)[itemIndex] == -1) {
		cerr << "item " << item << " has no tidlist" << endl;
		abort();
	}
	int32_t startPos = (*_indexAndFreqs)[itemIndex];
	int32_t length = (*_indexAndFreqs)[itemIndex + 1];
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
	uint32_t res = _tidlist->read(_startPos + _index);
	_index++;
	return res;
}

}
}
