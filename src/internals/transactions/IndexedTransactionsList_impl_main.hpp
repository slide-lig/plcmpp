
#include <internals/transactions/IndexedTransactionsList.hpp>
#include <internals/Counters.hpp>

#include <iostream>
using namespace std;

namespace internals {
namespace transactions {

template <class itemT>
const itemT IndexedTransactionsList<itemT>::MAX_VALUE = ~((itemT)0);

template <class itemT>
IndexedTransactionsList<itemT>::IndexedTransactionsList(
		Counters* c) : IndexedTransactionsList<itemT>(
				c->distinctTransactionLengthSum,
				c->distinctTransactionsCount)
{
}

template <class itemT>
IndexedTransactionsList<itemT>::IndexedTransactionsList(
		int32_t transactionsLength, int32_t nbTransactions)
{
	_concatenated = new itemT[transactionsLength];
	_transactions_info = new descTransaction[nbTransactions];
	_num_allocated_transactions = 0;
	_num_real_transactions = 0;
	_writeIndex = _concatenated;
}

template <class itemT>
IndexedTransactionsList<itemT>::~IndexedTransactionsList()
{
	delete[] _concatenated;
	delete[] _transactions_info;
}

template <class itemT>
int32_t IndexedTransactionsList<itemT>::getTransSupport(
		int32_t trans) {
	return _transactions_info[trans].support;
}

template <class itemT>
void IndexedTransactionsList<itemT>::setTransSupport(
		int32_t trans, int32_t s) {
	int32_t* p_support = &_transactions_info[trans].support;
	if (s != 0 && *p_support == 0) {
		_num_real_transactions++;
	} else if (s == 0 && *p_support != 0) {
		_num_real_transactions--;
	}
	*p_support = s;
}

template <class itemT>
unique_ptr<Iterator<int32_t> > IndexedTransactionsList<itemT>::getIdIterator() {
	return unique_ptr<Iterator<int32_t> >(new IdIter<itemT>(this));
}

template <class itemT>
unique_ptr<TransactionsWriter<itemT> > IndexedTransactionsList<itemT>::getWriter() {
	return unique_ptr<TransactionsWriter<itemT> >(new TransactionsWriter<itemT>(this));
}

template <class itemT>
int32_t IndexedTransactionsList<itemT>::beginTransaction(
		int32_t support, itemT*& write_index)
{
	_transactions_info[_num_allocated_transactions].start_transaction = _writeIndex;
	_transactions_info[_num_allocated_transactions].support = support;
	write_index = _writeIndex;
	if (support != 0) {
		++_num_real_transactions;
	}
	/* return the transaction id */
	return _num_allocated_transactions++;
}

template <class itemT>
void IndexedTransactionsList<itemT>::endTransaction(int32_t max_candidate, itemT* end_index) {
	descTransaction* desc_trans = _transactions_info + (_num_allocated_transactions -1);
	_writeIndex = end_index;
	desc_trans->end_transaction = _writeIndex;
	desc_trans->end_prefix =
			std::upper_bound(
					desc_trans->start_transaction,
					desc_trans->end_transaction,
					max_candidate
					);
	//cout << "itemT " << transId << endl;
	desc_trans->prefix_hash = SimpleDigest::digest(
			desc_trans->start_transaction,
			desc_trans->end_prefix);
}

template <class itemT>
int32_t IndexedTransactionsList<itemT>::findNext(int32_t nextPos) {
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

template <class itemT>
int32_t IndexedTransactionsList<itemT>::size() {
	return _num_real_transactions;
}

template <class itemT>
bool IndexedTransactionsList<itemT>::compatible(
		Counters* c) {
	return c->getMaxFrequent() <= MAX_VALUE;
}

template <class itemT>
int32_t IndexedTransactionsList<itemT>::getMaxTransId(
		Counters* c) {
	return c->distinctTransactionsCount - 1;
}

template<class itemT>
void IndexedTransactionsList<itemT>::countSubList(
		TidList::ItemTidList* tidlist,
		int32_t& transactionsCount,
		int32_t& distinctTransactionsCount,
		int32_t* supportCounts,
		int32_t* distinctTransactionsCounts,
		int32_t extension,
		int32_t maxItem) {

	// item support and transactions counting
	transactionsCount = 0;
	distinctTransactionsCount = 0;
	itemT *begin;
	itemT *end;
	itemT *it;
	itemT item;
	descTransaction* transaction_info;
	int32_t weight;

	auto tidlist_it = tidlist->iterator();

	while (tidlist_it->hasNext()) {

		transaction_info = &_transactions_info[tidlist_it->next()];
		weight = transaction_info->support;

		if (weight > 0) {
			begin = transaction_info->start_transaction;
			end = transaction_info->end_transaction;

			if (begin != end)
			{	// transaction is not empty
				transactionsCount += weight;
				++distinctTransactionsCount;
				for(it = begin; it < end; ++it)
				{
					item = *it;
					if (item <= maxItem) {
						supportCounts[item] += weight;
						distinctTransactionsCounts[item]++;
					}
					else
					{	// since transactions are ordered
						break;
					}
				}
			}
		}
	}

	// ignored items
	supportCounts[extension] = 0;
	distinctTransactionsCounts[extension] = 0;
}

template<class itemT>
template<class childItemT>
void IndexedTransactionsList<itemT>::copyTo(
		TidList::ItemTidList* item_tidList, TransactionsWriter<childItemT>* TransactionsWriter,
		TidList* new_tidList,
		int32_t* renaming, int32_t max_candidate) {

	itemT *begin;
	itemT *end;
	itemT *it;
	childItemT *write_index;
	int32_t item;
	int32_t transId;
	descTransaction* transaction_info;
	int32_t weight;

	auto item_tidList_it = item_tidList->iterator();

	while (item_tidList_it->hasNext()) {
		transaction_info = &_transactions_info[item_tidList_it->next()];

		weight = transaction_info->support;

		if (weight > 0) {
			begin = transaction_info->start_transaction;
			end = transaction_info->end_transaction;

			if (begin == end) continue;

			transId = TransactionsWriter->beginTransaction(weight, write_index);

			for(it = begin; it < end; ++it)
			{
				if (renaming == nullptr)
				{
					item = *it;
				}
				else
				{
					item = renaming[*it];
				}

				if (item != -1)
				{
					*(write_index++) = item;

					/* The next passes of the algorithm will only use
					 * the tidlists of items in the prefix */
					if (item < max_candidate)
					{
						new_tidList->addTransaction(item, transId);
					}
				}
			}

			TransactionsWriter->endTransaction(max_candidate, write_index);
		}
	}
}



template <class itemT>
TransactionsWriter<itemT>::TransactionsWriter(IndexedTransactionsList<itemT> *tlist) {
	_tlist = tlist;
}

template <class itemT>
int32_t TransactionsWriter<itemT>::beginTransaction(int32_t support, itemT*& write_index) {
	return _tlist->beginTransaction(support, write_index);
}

template <class itemT>
void TransactionsWriter<itemT>::endTransaction(int32_t max_candidate, itemT* end_index) {
	_tlist->endTransaction(max_candidate, end_index);
}

template <class itemT>
IdIter<itemT>::IdIter(IndexedTransactionsList<itemT>* tlist) {
	_tlist = tlist;
	pos = 0;
	nextPos = -1;
	findNext();
}

template <class itemT>
int32_t IdIter<itemT>::next() {
	pos = nextPos;
	findNext();
	return pos;
}

template <class itemT>
bool IdIter<itemT>::hasNext() {
	return nextPos != -1;
}

template <class itemT>
void IdIter<itemT>::findNext() {
	nextPos = _tlist->findNext(nextPos);
}

}
}
