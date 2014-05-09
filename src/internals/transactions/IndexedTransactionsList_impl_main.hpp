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

#include <internals/transactions/IndexedTransactionsList.hpp>
#include <internals/Counters.hpp>
#include <util/SimpleDigest.h>
#include <Config.hpp>
#include <internals/ExplorationStep.hpp>

#include <iostream>
using namespace std;

namespace internals {
namespace transactions {


#define MALLOC(type, num) 	(type*)malloc(num * sizeof(type))
#define REALLOC(var, num) 	(typeof(var))realloc(var, num * sizeof(*var))

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
{ 	// we use malloc because we may realloc later...
	_concatenated = MALLOC(itemT, transactionsLength);
	_end_concatenated = _concatenated + transactionsLength;
	_transactions_boundaries = MALLOC(transaction_boundaries_t, nbTransactions);
	_transactions_support = MALLOC(int32_t, nbTransactions);
	_num_transactions_allocated = nbTransactions;
	_num_transactions = 0;
	_writeIndex = _concatenated;
}

template <class itemT>
IndexedTransactionsList<itemT>::~IndexedTransactionsList()
{
	if (_concatenated != nullptr)
		free(_concatenated);
	if (_transactions_boundaries != nullptr)
		free(_transactions_boundaries);
	if (_transactions_support != nullptr)
		free(_transactions_support);
}

template <class itemT>
int32_t IndexedTransactionsList<itemT>::getTransSupport(
		int32_t trans) {
	return _transactions_boundaries[trans].support;
}

template <class itemT>
void IndexedTransactionsList<itemT>::incTransSupport(
		int32_t trans, int32_t s) {
	_transactions_support[trans] += s;
}

template <class itemT>
int32_t IndexedTransactionsList<itemT>::beginTransaction(
		int32_t support, itemT*& write_index)
{
	_transactions_boundaries[_num_transactions].start_transaction = _writeIndex;
	_transactions_support[_num_transactions] = support;
	write_index = _writeIndex;
	/* return the transaction id */
	return _num_transactions++;
}

template <class itemT>
typename IndexedTransactionsList<itemT>::transaction_boundaries_t*
			IndexedTransactionsList<itemT>::endTransaction(
					itemT* end_index) {
	transaction_boundaries_t* desc_trans = _transactions_boundaries + (_num_transactions -1);
	_writeIndex = end_index;
	desc_trans->end_transaction = _writeIndex;
	return desc_trans;
}

template <class itemT>
typename IndexedTransactionsList<itemT>::transaction_boundaries_t*
			IndexedTransactionsList<itemT>::endTransaction(
		itemT* end_index, int32_t max_candidate,
		itemT* &end_prefix) {

	transaction_boundaries_t* desc_trans = endTransaction(end_index);
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
	_writeIndex = _transactions_boundaries[_num_transactions -1].end_transaction;
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
void IndexedTransactionsList<itemT>::repack() {

	bool should_realloc_concatenated = false;
	bool should_realloc_trans_info = false;

	if (_num_transactions == 0)
	{
		free(_transactions_boundaries);
		free(_transactions_support);
		_transactions_boundaries = nullptr;
		_transactions_support = nullptr;
	}
	else
	{
		double over_alloc = ((double)_num_transactions_allocated) /
								_num_transactions;

		if (over_alloc > Config::OVER_ALLOC_THRESHOLD)
		{
			should_realloc_trans_info = true;
		}
	}

	if (_writeIndex - _concatenated == 0)
	{
		free(_concatenated);
		_concatenated = nullptr;
	}
	else
	{
		double over_alloc = ((double)(_end_concatenated - _concatenated)) /
					(_writeIndex - _concatenated);

		if (over_alloc > Config::OVER_ALLOC_THRESHOLD)
		{
			should_realloc_concatenated = true;
		}
	}

	if (should_realloc_trans_info)
	{
		// realloc things
		_transactions_boundaries = REALLOC(_transactions_boundaries,
										_num_transactions);
		_transactions_support = REALLOC(_transactions_support,
										_num_transactions);

		// update things
		_num_transactions_allocated = _num_transactions;
	}

	if (should_realloc_concatenated)
	{
		// realloc a minimal table
		auto size = _writeIndex - _concatenated;
		itemT *_concatenated_new = REALLOC(_concatenated, size);

		if (_concatenated_new != _concatenated)
		{
			// update pointers to the old table
			auto offset = _concatenated_new - _concatenated;
			auto it_start = (itemT**)(_transactions_boundaries);
			auto it_end = (itemT**)(_transactions_boundaries + _num_transactions);
			for (auto it = it_start; it != it_end; ++it)
			{
				*it += offset;
			}

			// update things
			_concatenated = _concatenated_new;
		}

		// update things
		_end_concatenated = _concatenated + size;
		_writeIndex = _end_concatenated;
	}
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
	transaction_boundaries_t* transaction_info;
	int32_t weight;
	int32_t tid;

	auto tidlist_it = tidlist->iterator();

	while (tidlist_it->hasNext()) {

		tid = tidlist_it->next();
		transaction_info = &_transactions_boundaries[tid];

		begin = transaction_info->start_transaction;
		end = transaction_info->end_transaction;

		if (begin != end)
		{	// transaction is not empty
			weight = _transactions_support[tid];
			transactionsCount += weight;
			++distinctTransactionsCount;
			for(it = begin; it < end; ++it)
			{
				item = *it;
				if ((int32_t)item <= maxItem) {
					supportCounts[item] += weight;
					++(distinctTransactionsCounts[item]);
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

	int32_t tid;
	itemT *begin, *end, *it;
	childItemT *write_index, *end_prefix;
	int32_t item, transId, weight;
	transaction_boundaries_t* transaction_info;
	typename PrefixDeduplication<childItemT>::prefix_set_t known_prefixes_info;

#ifdef DEBUG_STEP
	if (ExplorationStep::next_id == DEBUG_STEP)
	{
		cout << "Uncompressed: (max_candidate = " << max_candidate << ")" << endl;
	}
#endif

	auto item_tidList_it = item_tidList->iterator();

	while (item_tidList_it->hasNext()) {
		tid = item_tidList_it->next();
		transaction_info = &_transactions_boundaries[tid];

		begin = transaction_info->start_transaction;
		end = transaction_info->end_transaction;

		if (begin == end) continue;

		weight = _transactions_support[tid];

		transId = writer->beginTransaction(weight, write_index);

#ifdef DEBUG_STEP
		if (ExplorationStep::next_id == DEBUG_STEP)
		{
			cout << "[ ";
		}
#endif

		if (renaming == nullptr)
		{
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
#ifdef DEBUG_STEP
					if (ExplorationStep::next_id == DEBUG_STEP)
					{
						cout << item << " ";
					}
#endif
				}
			}
		}

#ifdef DEBUG_STEP
		if (ExplorationStep::next_id == DEBUG_STEP)
		{
			cout << "]" << endl;
		}
#endif

		auto trans_desc = writer->endTransaction(write_index, max_candidate, end_prefix);

		PrefixDeduplication<childItemT>::insertOrMerge(
				known_prefixes_info,
				transId,
				weight,
				trans_desc,
				end_prefix,
				writer);
	}

#ifdef DEBUG_STEP
	if (ExplorationStep::next_id == DEBUG_STEP)
	{
		cout << endl;
		cout << "Compressed: " << endl;
		writer->print();
		cout << endl;
	}
#endif

	if (Config::AVOID_OVER_ALLOC)
	{
		writer->repack();
	}

	writer->updateTidList(new_tidList);
}

template<class itemT>
void IndexedTransactionsList<itemT>::updateTidList(TidList* new_tidList)
{
	int32_t tid;
	itemT *begin, *end, *it;
	transaction_boundaries_t* transaction_info;

	transaction_info = _transactions_boundaries;

	for (tid = 0; tid < _num_transactions; ++tid)
	{
		begin = transaction_info->start_transaction;
		end = transaction_info->end_transaction;

		for(it = begin; it < end; ++it)
		{
			new_tidList->addTransaction(*it, tid);
		}

		++transaction_info;
	}
}

template<class itemT>
template <class otherItemT>
IndexedTransactionsList<itemT>::IndexedTransactionsList(
		const IndexedTransactionsList<otherItemT>* other,
		TidList* updatable_tidlist) {

	auto len = other->_end_concatenated - other->_concatenated;
	_concatenated = MALLOC(itemT, len);
	_end_concatenated = _concatenated + len;
	_transactions_boundaries = MALLOC(transaction_boundaries_t, other->_num_transactions);
	_transactions_support = MALLOC(int32_t, other->_num_transactions);
	_num_transactions_allocated = other->_num_transactions;
	_num_transactions = 0;
	_writeIndex = _concatenated;

	otherItemT *begin, *end;
	itemT *end_prefix, *write_index;
	int32_t transId, weight;
	transaction_boundaries_t* transaction_info;
	typename PrefixDeduplication<itemT>::prefix_set_t known_prefixes_info;

	transaction_info = other->_transactions_boundaries;

	for (auto tid = 0; tid < other->_num_transactions; ++tid)
	{
		begin = transaction_info->start_transaction;
		end = transaction_info->end_transaction;

		if (begin == end) continue;

		weight = other->_transactions_support[tid];

		transId = this->beginTransaction(weight, write_index);

		write_index = std::copy(begin, end, write_index);

		auto trans_desc = this->endTransaction(write_index, INT32_MAX, end_prefix);

		PrefixDeduplication<itemT>::insertOrMerge(
				known_prefixes_info,
				transId,
				weight,
				trans_desc,
				end_prefix,
				this);

		++transaction_info;
	}

	cout << "Found " << (other->_num_transactions - _num_transactions) << "/" <<
			other->_num_transactions <<
			" duplicated transactions." << endl;

	if (Config::AVOID_OVER_ALLOC)
	{
		this->repack();
	}

	updatable_tidlist->resetTidLists();
	updateTidList(updatable_tidlist);
}

template<class itemT>
void IndexedTransactionsList<itemT>::print()
{
	int32_t tid;
	itemT *begin, *end, *it;
	transaction_boundaries_t* transaction_info;

	transaction_info = _transactions_boundaries;

	for (tid = 0; tid < _num_transactions; ++tid)
	{
		begin = transaction_info->start_transaction;
		end = transaction_info->end_transaction;

		cout << "[ ";
		for(it = begin; it < end; ++it)
		{
			cout << (int)(*it) << " ";
		}
		cout << "]" << endl;

		++transaction_info;
	}
}

}
}
