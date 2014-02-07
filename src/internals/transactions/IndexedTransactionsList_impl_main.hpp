
#include <internals/transactions/IndexedTransactionsList.hpp>
#include <internals/transactions/IndexedTransactionsList_iterators.hpp>

#include <iostream>
using namespace std;

namespace internals {
namespace transactions {

template <class T>
const T IndexedTransactionsList<T>::MAX_VALUE = ~((T)0);

template <class T>
IndexedTransactionsList<T>::IndexedTransactionsList(
		Counters* c) : IndexedTransactionsList<T>(
				c->distinctTransactionLengthSum,
				c->distinctTransactionsCount)
{
}

template <class T>
IndexedTransactionsList<T>::IndexedTransactionsList(
		int32_t transactionsLength, int32_t nbTransactions)
{
	_concatenated = new T[transactionsLength];
	_transactions_info = new descTransaction[nbTransactions];
	_num_allocated_transactions = 0;
	_num_real_transactions = 0;
	_writeIndex = _concatenated;
}

template <class T>
IndexedTransactionsList<T>::~IndexedTransactionsList()
{
	delete[] _concatenated;
	delete[] _transactions_info;
}

template <class T>
int32_t IndexedTransactionsList<T>::getTransSupport(
		int32_t trans) {
	return _transactions_info[trans].support;
}

template <class T>
void IndexedTransactionsList<T>::setTransSupport(
		int32_t trans, int32_t s) {
	int32_t* p_support = &_transactions_info[trans].support;
	if (s != 0 && *p_support == 0) {
		_num_real_transactions++;
	} else if (s == 0 && *p_support != 0) {
		_num_real_transactions--;
	}
	*p_support = s;
}

template <class T>
unique_ptr<Iterator<int32_t> > IndexedTransactionsList<T>::getIdIterator() {
	return unique_ptr<Iterator<int32_t> >(new IdIter<T>(this));
}

template <class T>
unique_ptr<TransactionsWriter> IndexedTransactionsList<T>::getWriter() {
	return unique_ptr<TransactionsWriter>(new Writer<T>(this));
}

template <class T>
void IndexedTransactionsList<T>::beginTransaction(int32_t transId,
		int32_t support) {
	_transactions_info[transId].start_transaction = _writeIndex;
	_transactions_info[transId].support = support;
	if (support != 0) {
		++_num_real_transactions;
	}
	++_num_allocated_transactions;
}

template <class T>
void IndexedTransactionsList<T>::endTransaction(int32_t transId, int32_t core_item) {
	descTransaction* desc_trans = _transactions_info + transId;
	desc_trans->end_transaction = _writeIndex;
	desc_trans->end_prefix =
			std::upper_bound(
					desc_trans->start_transaction,
					desc_trans->end_transaction,
					core_item
					);
	//cout << "t " << transId << endl;
	desc_trans->prefix_hash = SimpleDigest::digest(
			desc_trans->start_transaction,
			desc_trans->end_prefix);
}

template <class T>
int32_t IndexedTransactionsList<T>::findNext(int32_t nextPos) {
	while (true) {
		nextPos++;
		if (nextPos >= _num_allocated_transactions) {
			return -1;
		}
		// cout << "num: " << _num_allocated_transactions <<
		//   " nextpos: " << nextPos << endl;
		if (_transactions_info[nextPos].support > 0) {
			return nextPos;
		}
	}
	return 0; // never reached, avoids a warning
}

template <class T>
int32_t IndexedTransactionsList<T>::size() {
	return _num_real_transactions;
}

template <class T>
template <class IteratorT>
unique_ptr<Template_ReusableTransactionIterator<IteratorT> > IndexedTransactionsList<T>::getIteratorWithType() {
	return unique_ptr<Template_ReusableTransactionIterator<IteratorT> >(
			new Template_TransIter<T, IteratorT>(this));
}

template <class T>
template <class IteratorT>
void IndexedTransactionsList<T>::positionIterator(
		int32_t transaction,
		Template_IndexedReusableIterator<T, IteratorT>* iter)
{
	iter->set(
			_transactions_info[transaction].start_transaction,
			_transactions_info[transaction].end_transaction);
}

template <class T>
unique_ptr<ReusableTransactionIterator> IndexedTransactionsList<T>::getIterator() {
	return getIteratorWithType<int32_t>();
}

template <class T>
bool IndexedTransactionsList<T>::compatible(
		Counters* c) {
	return c->getMaxFrequent() <= MAX_VALUE;
}

template <class T>
int32_t IndexedTransactionsList<T>::getMaxTransId(
		Counters* c) {
	return c->distinctTransactionsCount - 1;
}

template <class T>
void IndexedTransactionsList<T>::writeItem(
		int32_t item) {
	if (item > MAX_VALUE) {
		cerr << item <<
				" too big for this kind of transaction list! Aborting."
				<< endl;
		abort();
	}
	*_writeIndex = (T) item;
	_writeIndex++;
}

template <class T>
Writer<T>::Writer(IndexedTransactionsList<T> *tlist) {
	transId = 0;
	_tlist = tlist;
}

template <class T>
int32_t Writer<T>::beginTransaction(int32_t support) {
	_tlist->beginTransaction(transId, support);
	return transId;
}

template <class T>
void Writer<T>::addItem(int32_t item) {
	_tlist->writeItem(item);
}

template <class T>
void Writer<T>::endTransaction(int32_t core_item) {
	_tlist->endTransaction(transId++, core_item);
}

template <class T>
IdIter<T>::IdIter(IndexedTransactionsList<T>* tlist) {
	_tlist = tlist;
	pos = 0;
	nextPos = -1;
	findNext();
}

template <class T>
int32_t IdIter<T>::next() {
	pos = nextPos;
	findNext();
	return pos;
}

template <class T>
bool IdIter<T>::hasNext() {
	return nextPos != -1;
}

template <class T>
void IdIter<T>::findNext() {
	nextPos = _tlist->findNext(nextPos);
}

}
}
