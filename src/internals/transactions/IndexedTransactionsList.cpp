
#include <internals/transactions/IndexedTransactionsList.hpp>

#include <iostream>
using namespace std;

namespace internals {
namespace transactions {

// constructor
IndexedTransactionsList::IndexedTransactionsList(
		int32_t nbTransactions) {
	_indexAndFreqs = new vector<int32_t>(nbTransactions << 1, -1);
	_size = 0;
	writeIndex = 0;
}

// copy constructor
IndexedTransactionsList::IndexedTransactionsList(
		const IndexedTransactionsList& other)
{
	_size = other._size;
	writeIndex = other.writeIndex;
	_indexAndFreqs = other._indexAndFreqs;
}

void IndexedTransactionsList::positionIterator(
		int32_t transaction, IndexedReusableIterator* iter) {

	uint32_t startPos = transaction << 1;
	if (startPos >= _indexAndFreqs->size() ||
			(*_indexAndFreqs)[startPos] == -1) {
		cerr << "transaction " << transaction <<
				" does not exist! Aborting." << endl;
		abort();
	} else {
		uint32_t endPos = startPos + 2;
		int32_t end;
		if (endPos < _indexAndFreqs->size()) {
			end = (*_indexAndFreqs)[endPos];
			if (end == -1) {
				end = writeIndex;
			}
		} else {
			end = writeIndex;
		}
		iter->set((*_indexAndFreqs)[startPos], end);
	}
}

int32_t IndexedTransactionsList::getTransSupport(
		int32_t trans) {
	int32_t startPos = trans << 1;
	return (*_indexAndFreqs)[startPos + 1];
}

void IndexedTransactionsList::setTransSupport(
		int32_t trans, int32_t s) {
	int32_t startPos = trans << 1;
	if (s != 0 && (*_indexAndFreqs)[startPos + 1] == 0) {
		_size++;
	} else if (s == 0 && (*_indexAndFreqs)[startPos + 1] != 0) {
		_size--;
	}
	(*_indexAndFreqs)[startPos + 1] = s;
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
	(*_indexAndFreqs)[startPos] = writeIndex;
	(*_indexAndFreqs)[startPos + 1] = support;
	if (support != 0) {
		_size++;
	}
}

void IndexedTransactionsList::findNext(int32_t nextPos) {
	while (true) {
		nextPos++;
		uint32_t nextPosStart = nextPos << 1;
		if (nextPosStart >= _indexAndFreqs->size() ||
				(*_indexAndFreqs)[nextPosStart] == -1) {
			nextPos = -1;
			return;
		}
		if ((*_indexAndFreqs)[nextPosStart + 1] > 0) {
			return;
		}
	}
}

int32_t IndexedTransactionsList::size() {
	return _size;
}

IndexedReusableIterator::IndexedReusableIterator(
		IndexedTransactionsList* tlist) {
	_tlist = tlist;
	transNum = 0;
}

void IndexedReusableIterator::setTransaction(
		int32_t transaction) {
	transNum = transaction;
	_tlist->positionIterator(transaction, this);
}

int32_t IndexedReusableIterator::getTransactionSupport() {
	return _tlist->getTransSupport(transNum);
}

void IndexedReusableIterator::setTransactionSupport(
		int32_t s) {
	_tlist->setTransSupport(transNum, s);
}

BasicTransIter::BasicTransIter(IndexedTransactionsList* tlist)
	: IndexedReusableIterator(tlist) {
	_first = true;
	_end = _pos = _nextPos = 0;
}

void BasicTransIter::set(int32_t begin, int32_t end) {
	_nextPos = begin - 1;
	_end = end;
	_first = true;
}

void BasicTransIter::findNext() {
	while (true) {
		_nextPos++;
		if (_nextPos == _end) {
			_nextPos = -1;
			return;
		}
		if (isNextPosValid()) {
			return;
		}
	}
}

int32_t BasicTransIter::next() {
	_pos = _nextPos;
	findNext();
	return getPosVal();
}

bool BasicTransIter::hasNext() {
	if (_first) {
		_first = false;
		findNext();
	}
	return _nextPos != -1;
}

void BasicTransIter::remove() {
	removePosVal();
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

int32_t& IdIter::next() {
	pos = nextPos;
	findNext();
	return pos;
}

bool IdIter::hasNext() {
	return nextPos != -1;
}

void IdIter::findNext() {
	_tlist->findNext(nextPos);
}

}
}
