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

#pragma once

#include <internals/transactions/TransactionsList.hpp>
#include <util/Iterator.hpp>
#include <util/shortcuts.h>

using namespace util;

namespace internals {
namespace transactions {

template <class itemT>
class IndexedTransactionsList: public TransactionsList {

public:
	typedef itemT* trans_start_t;
	typedef itemT* prefix_end_t;
	typedef itemT* trans_end_t;

	typedef struct {
		trans_start_t start_transaction;
		trans_end_t end_transaction;
	} transaction_boundaries_t;

protected:
	itemT* _concatenated;
	itemT* _end_concatenated;

	transaction_boundaries_t* _transactions_boundaries;
	int32_t* _transactions_support;
	int32_t _num_transactions;
	int32_t _num_transactions_allocated;
	itemT* _writeIndex;

public:
	typedef itemT base_type;
	static const itemT MAX_VALUE;

	IndexedTransactionsList(Counters* c);
	IndexedTransactionsList(int32_t transactionsLength,
			int32_t nbTransactions);
	template <class otherItemT>
	IndexedTransactionsList(
			const IndexedTransactionsList<otherItemT>* other,
			TidList* new_tidList);
	~IndexedTransactionsList();

	int32_t getTransSupport(int32_t trans);
	void incTransSupport(int32_t trans, int32_t s);
	int32_t beginTransaction(int32_t support, itemT*& write_index);
	transaction_boundaries_t* endTransaction(itemT* end_index);
	transaction_boundaries_t* endTransaction(itemT* end_index,
			int32_t max_candidate, itemT* &end_prefix);
	void discardLastTransaction();
	void repack();
	int32_t size() override;

	static bool compatible(Counters* c);
	static int32_t getMaxTransId(Counters* c);

	void countSubList(TidList::ItemTidList *tidlist,
	    		int32_t& transactionsCount, int32_t& distinctTransactionsCount,
	    		int32_t* supportCounts, int32_t* distinctTransactionsCounts,
	    		int32_t extension, int32_t maxItem) override;

	template <class childItemT>
	void copyTo(TidList::ItemTidList* item_tidList,
			IndexedTransactionsList<childItemT>* writer, TidList* new_tidList,
	    		int32_t* renaming, int32_t max_candidate);

	void updateTidList(TidList* new_tidList);

	void print();
};

}
}

/* Template classes, we need their definition */
#include <internals/transactions/IndexedTransactionsList_impl_compress.hpp>
#include <internals/transactions/IndexedTransactionsList_impl_main.hpp>

