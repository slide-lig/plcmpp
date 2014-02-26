#pragma once

#include <climits>
#include <memory>
using namespace std;

#include <internals/tidlist/Template_ConsecutiveItemsConcatenatedTidList.hpp>
#include "internals/transactions/IndexedTransactionsList.hpp"
#include "internals/tidlist/TidList.hpp"
#include "util/Iterator.hpp"
#include "util/shortcuts.h"
#include "PLCM.hpp"
#include "internals/DatasetFactory.hpp"

using namespace internals::transactions;
using namespace internals::tidlist;
using namespace util;

namespace internals {

class Counters;

/**
 * Stores transactions and does occurrence delivery
 */

class TransactionsSubList;

template<class TransactionsListT, class TidListT>
class SpecializedDataset;

class Dataset {
public:
	virtual ~Dataset();
	virtual unique_ptr<TransactionsSubList> getTransactionsSubList(
			int32_t item) = 0;
	virtual void countSubList(TidList::ItemTidList *item_tidlist,
			int32_t &transactionsCount, int32_t &distinctTransactionsCount,
			int32_t *supportCounts, int32_t *distinctTransactionsCounts,
			int32_t extension, int32_t maxItem) = 0;
	virtual unique_ptr<Dataset> instanciateChildDataset(int32_t extension,
			shp_array_int32 renaming, Counters *counters,
			int32_t max_candidate) = 0;
	virtual void compress() = 0;
	virtual bool checkTidListsInclusion(int32_t extension, int32_t i) = 0;

	template<class TransactionsListT, class TidListT>
	static unique_ptr<Dataset> instanciateDataset(
			unique_ptr<TransactionsListT> trnlist,
			unique_ptr<TidListT> tidlist) {
		return unique_ptr<Dataset>(
				new SpecializedDataset<TransactionsListT, TidListT>(
						// pass ownership of these 2 unique_ptr
						std::move(trnlist),
						std::move(tidlist)));
	}

};

typedef DatasetFactoryImpl<Dataset> DatasetFactory;

class TransactionsSubList {
protected:
	unique_ptr<TidList::ItemTidList> _tids;
	Dataset *_dataset;

public:
	TransactionsSubList(Dataset *dataset,
			unique_ptr<TidList::ItemTidList> tidList);
	void count(int32_t &transactionsCount, int32_t &distinctTransactionsCount,
			int32_t *supportCounts, int32_t *distinctTransactionsCounts,
			int32_t extension, int32_t maxItem);
};

template<class TransactionsListT, class TidListT>
class SpecializedDataset: public Dataset {
protected:
	unique_ptr<TransactionsListT> _transactions;

	/**
	 * frequent item => array of occurrences indexes in "concatenated"
	 * Transactions are added in the same order in all occurrences-arrays.
	 */
	unique_ptr<TidListT> _tidList;

public:
	typedef SpecializedDataset<TransactionsListT, TidListT> self_type;
	typedef typename TransactionsListT::base_type item_type;
	typedef typename TidListT::base_type tid_type;

	SpecializedDataset(unique_ptr<TransactionsListT> trnlist,
			unique_ptr<TidListT> tidlist) {
		// transfer ownership
		_transactions = std::move(trnlist);
		_tidList = std::move(tidlist);
	}

	unique_ptr<TransactionsSubList> getTransactionsSubList(int32_t item)
			override {
		return unique_ptr<TransactionsSubList>(
				new TransactionsSubList(this, _tidList->getItemTidList(item)));
	}

	unique_ptr<TidList::ItemTidList> getItemTidList(int32_t item) {
		return _tidList->getItemTidList(item);
	}

	void countSubList(TidList::ItemTidList *tidlist, int32_t& transactionsCount,
			int32_t& distinctTransactionsCount, int32_t* supportCounts,
			int32_t* distinctTransactionsCounts, int32_t extension,
			int32_t maxItem) override {

		_transactions->countSubList(tidlist, transactionsCount,
				distinctTransactionsCount, supportCounts,
				distinctTransactionsCounts, extension, maxItem);
	}

	template<class childItemT>
	void copyTo(TidList::ItemTidList* item_tidList,
			IndexedTransactionsList<childItemT>* writer, TidList* new_tidList,
			int32_t* renaming, int32_t max_candidate) {
		_transactions->copyTo(item_tidList, writer, new_tidList, renaming,
				max_candidate);
	}

	unique_ptr<Dataset> instanciateChildDataset(int32_t extension,
			shp_array_int32 renaming, Counters *counters, int32_t max_candidate)
					override {

		return DatasetFactory::initChildDataset<self_type>(
						this, extension, renaming->begin(), counters, max_candidate);
	}

	void compress() override {
		TransactionsListT *trn_list = new TransactionsListT(_transactions.get(), _tidList.get());
		_transactions.reset(trn_list);
	}

	bool checkTidListsInclusion(int32_t extension, int32_t i) override {
		return _tidList->checkTidListsInclusion(extension, i);
	}
};

}
