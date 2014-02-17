

#include "PLCM.hpp"
#include "internals/Dataset.hpp"
#include "internals/Counters.hpp"

namespace internals {

// TransactionsSubList class
// --------------------------
TransactionsSubList::TransactionsSubList(
		Dataset *dataset,
		unique_ptr<TidList::ItemTidList> tidList) {
	_tids = std::move(tidList);
	_dataset = dataset;
}

void TransactionsSubList::count(int32_t& transactionsCount,
		int32_t& distinctTransactionsCount, int32_t* supportCounts,
		int32_t* distinctTransactionsCounts,
		int32_t extension, int32_t maxItem) {

	_dataset->countSubList(_tids.get(),
			transactionsCount,	distinctTransactionsCount,
			supportCounts, distinctTransactionsCounts, extension, maxItem);
}

}
