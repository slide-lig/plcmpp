
#include <internals/transactions/IndexedTransactionsList.hpp>

#include <iostream>
using namespace std;

namespace internals {
namespace transactions {

// constructor
IndexedTransactionsList::IndexedTransactionsList(
		int32_t nbTransactions) {
	_indexAndFreqs = new array_int32(nbTransactions << 1);
	_indexAndFreqs_fast = _indexAndFreqs->array;
	_indexAndFreqs_used_size = 0;
	_size = 0;
	writeIndex = 0;
}

// copy constructor
IndexedTransactionsList::IndexedTransactionsList(
		const IndexedTransactionsList& other)
{
	_size = other._size;
	writeIndex = other.writeIndex;
	_indexAndFreqs = new array_int32(*(other._indexAndFreqs));
	_indexAndFreqs_fast = _indexAndFreqs->array;
	_indexAndFreqs_used_size = other._indexAndFreqs_used_size;
}

IndexedTransactionsList::~IndexedTransactionsList() {
	delete _indexAndFreqs;
}

int32_t IndexedTransactionsList::getTransSupport(
		int32_t trans) {
	int32_t startPos = trans << 1;
	return _indexAndFreqs_fast[startPos + 1];
}

void IndexedTransactionsList::setTransSupport(
		int32_t trans, int32_t s) {
	int32_t* p_support = _indexAndFreqs_fast + ((trans << 1) + 1);
	if (s != 0 && *p_support == 0) {
		_size++;
	} else if (s == 0 && *p_support != 0) {
		_size--;
	}
	*p_support = s;
}

unique_ptr<Iterator<int32_t> > IndexedTransactionsList::getIdIterator() {
	return unique_ptr<Iterator<int32_t> >(new IdIter(this));
}

unique_ptr<TransactionsWriter> IndexedTransactionsList::getWriter() {
	return unique_ptr<TransactionsWriter>(new Writer(this));
}

void IndexedTransactionsList::beginTransaction(int32_t transId,
		int32_t support) {
	int32_t startPos = transId << 1;
	_indexAndFreqs_fast[startPos] = writeIndex;
	_indexAndFreqs_fast[startPos + 1] = support;
	if (support != 0) {
		++_size;
	}
	++_indexAndFreqs_used_size;
}

int32_t IndexedTransactionsList::findNext(int32_t nextPos) {
	while (true) {
		nextPos++;
		if (nextPos >= _indexAndFreqs_used_size) {
			return -1;
		}
		if (_indexAndFreqs_fast[(nextPos << 1) + 1] > 0) {
			return nextPos;
		}
	}
	return 0; // never reached, avoids a warning
}

int32_t IndexedTransactionsList::size() {
	return _size;
}

Writer::Writer(IndexedTransactionsList* tlist) {
	_tlist = tlist;
	transId = -1;
}

int32_t Writer::beginTransaction(int32_t support) {
	transId++;
	_tlist->beginTransaction(transId, support);
	return transId;
}

void Writer::addItem(int32_t item) {
	_tlist->writeItem(item);
}

void Writer::endTransaction() {
}

IdIter::IdIter(IndexedTransactionsList* tlist) {
	_tlist = tlist;
	pos = 0;
	nextPos = -1;
	findNext();
}

int32_t IdIter::next() {
	pos = nextPos;
	findNext();
	return pos;
}

bool IdIter::hasNext() {
	return nextPos != -1;
}

void IdIter::findNext() {
	nextPos = _tlist->findNext(nextPos);
}

}
}
