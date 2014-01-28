
#include <iostream>
using namespace std;

#include <internals/transactions/Template_IndexedTransactionsList.hpp>
#include <internals/Counters.hpp>

namespace internals {
namespace transactions {

template <class T>
const T Template_IndexedTransactionsList<T>::MAX_VALUE = ~((T)0);

template <class T>
Template_IndexedTransactionsList<T>::Template_IndexedTransactionsList(
		Counters* c) : Template_IndexedTransactionsList<T>(
				c->distinctTransactionLengthSum,
				c->distinctTransactionsCount)
{
}

template <class T>
Template_IndexedTransactionsList<T>::Template_IndexedTransactionsList(
		int32_t transactionsLength, int32_t nbTransactions) :
		IndexedTransactionsList(nbTransactions)
{
	_concatenated = new RawArray<T>(transactionsLength);
	_concatenated_fast = _concatenated->array;
}

template <class T>
Template_IndexedTransactionsList<T>::Template_IndexedTransactionsList(
		const Template_IndexedTransactionsList<T>& other) :
				IndexedTransactionsList(other) {
	_concatenated = new RawArray<T>(*(other._concatenated));
	_concatenated_fast = _concatenated->array;
}

template <class T>
Template_IndexedTransactionsList<T>::~Template_IndexedTransactionsList()
{
	delete _concatenated;
}

template <class T>
unique_ptr<ReusableTransactionIterator> Template_IndexedTransactionsList<T>::getIterator() {
	return unique_ptr<ReusableTransactionIterator>(
			new Template_TransIter<T>(this, _concatenated_fast));
}

template <class T>
unique_ptr<TransactionsList> Template_IndexedTransactionsList<T>::clone() {
	// create a clone using the copy constructor
	Template_IndexedTransactionsList<T> *cloned = new
			Template_IndexedTransactionsList<T>(*this);
	return unique_ptr<TransactionsList>(cloned);
}

template <class T>
bool Template_IndexedTransactionsList<T>::compatible(
		Counters* c) {
	return c->getMaxFrequent() < MAX_VALUE;
}

template <class T>
int32_t Template_IndexedTransactionsList<T>::getMaxTransId(
		Counters* c) {
	return c->distinctTransactionsCount - 1;
}

template <class T>
void Template_IndexedTransactionsList<T>::writeItem(
		int32_t item) {
	if (item >= MAX_VALUE) {
		cerr << item <<
				" too big for this kind of transaction list! Aborting."
				<< endl;
		abort();
	}
	_concatenated_fast[writeIndex] = (T) item;
	writeIndex++;
}

}
}
