
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
		int32_t support;
		trans_start_t start_transaction;
		trans_end_t end_transaction;
	} descTransaction;

protected:
	itemT* _concatenated;

	descTransaction* _transactions_info;
	int32_t _num_transactions;
	int32_t _num_real_transactions;
	itemT* _writeIndex;

public:
	typedef itemT base_type;
	static const itemT MAX_VALUE;

	IndexedTransactionsList(Counters* c);
	IndexedTransactionsList(int32_t transactionsLength,
			int32_t nbTransactions);
	~IndexedTransactionsList();

	int32_t getTransSupport(int32_t trans);
	void incTransSupport(int32_t trans, int32_t s);
	int32_t beginTransaction(int32_t support, itemT*& write_index);
	descTransaction* endTransaction(itemT* end_index);
	descTransaction* endTransaction(itemT* end_index,
			int32_t max_candidate, itemT* &end_prefix);
	void discardLastTransaction();
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

private:
    static void sort(
    		descTransaction* transactions_info,
    		int32_t* start, int32_t* end);
    static int32_t merge(
    		descTransaction* transactions_info,
    		int32_t t1,
    		int32_t t2);

};

}
}

/* Template classes, we need their definition */
#include <internals/transactions/IndexedTransactionsList_impl_compress.hpp>
#include <internals/transactions/IndexedTransactionsList_impl_main.hpp>

