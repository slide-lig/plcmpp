
#include <internals/transactions/IndexedTransactionsList.hpp>
#include <internals/Counters.hpp>
#include <util/SimpleDigest.h>
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
	_num_transactions = 0;
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
void IndexedTransactionsList<itemT>::incTransSupport(
		int32_t trans, int32_t s) {
	_transactions_info[trans].support += s;
}

template <class itemT>
int32_t IndexedTransactionsList<itemT>::beginTransaction(
		int32_t support, itemT*& write_index)
{
	_transactions_info[_num_transactions].start_transaction = _writeIndex;
	_transactions_info[_num_transactions].support = support;
	write_index = _writeIndex;
	/* return the transaction id */
	return _num_transactions++;
}

template <class itemT>
typename IndexedTransactionsList<itemT>::descTransaction*
			IndexedTransactionsList<itemT>::endTransaction(
					itemT* end_index) {
	descTransaction* desc_trans = _transactions_info + (_num_transactions -1);
	_writeIndex = end_index;
	desc_trans->end_transaction = _writeIndex;
	return desc_trans;
}

template <class itemT>
typename IndexedTransactionsList<itemT>::descTransaction*
			IndexedTransactionsList<itemT>::endTransaction(
		itemT* end_index, int32_t max_candidate,
		itemT* &end_prefix) {

	descTransaction* desc_trans = endTransaction(end_index);
	end_prefix = std::upper_bound(
					desc_trans->start_transaction,
					desc_trans->end_transaction,
					max_candidate
					);
	return desc_trans;
}

template<class itemT>
void IndexedTransactionsList<itemT>::discardLastTransaction() {
	_num_transactions -= 1;
	_writeIndex = _transactions_info[_num_transactions -1].end_transaction;
}

template <class itemT>
int32_t IndexedTransactionsList<itemT>::size() {
	return _num_transactions;
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

		begin = transaction_info->start_transaction;
		end = transaction_info->end_transaction;

		if (begin != end)
		{	// transaction is not empty
			weight = transaction_info->support;
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

	// ignored items
	supportCounts[extension] = 0;
	distinctTransactionsCounts[extension] = 0;
}

template<class itemT>
template<class childItemT>
void IndexedTransactionsList<itemT>::copyTo(
		TidList::ItemTidList* item_tidList, IndexedTransactionsList<childItemT>* writer,
		TidList* new_tidList,
		int32_t* renaming, int32_t max_candidate) {

	itemT *begin, *end, *it;
	childItemT *write_index, *start_transaction, *end_prefix, *it_child;
	int32_t item, transId, weight;
	descTransaction* transaction_info;
	typename PrefixDeduplication<childItemT>::prefix_set_t known_prefixes_info;

	auto item_tidList_it = item_tidList->iterator();

	while (item_tidList_it->hasNext()) {
		transaction_info = &_transactions_info[item_tidList_it->next()];

		begin = transaction_info->start_transaction;
		end = transaction_info->end_transaction;

		if (begin == end) continue;

		weight = transaction_info->support;

		transId = writer->beginTransaction(weight, write_index);
		start_transaction = write_index;

		if (renaming == nullptr)
		{
			item = *it;
			write_index = std::copy(begin, end, write_index);
		}
		else
		{
			for(it = begin; it < end; ++it)
			{
				item = renaming[*it];

				if (item != -1)
				{
					*(write_index++) = item;
				}
			}
		}

		auto trans_desc = writer->endTransaction(write_index, max_candidate, end_prefix);

		if (PrefixDeduplication<childItemT>::insertOrMerge(
				known_prefixes_info,
				transId,
				weight,
				trans_desc,
				end_prefix,
				writer))
		{	// insertion OK: prefix not met yet

			/* The next passes of the algorithm will only use
			 * the tidlists of items in the prefix */
			for(	it_child = start_transaction;
					it_child != end_prefix;
					it_child++)
			{
				new_tidList->addTransaction(*it_child, transId);
			}
		}
	}
}

}
}
