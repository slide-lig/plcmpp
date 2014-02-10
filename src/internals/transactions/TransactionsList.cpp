
#include <algorithm>
using namespace std;

#include <internals/transactions/TransactionsList.hpp>
#include <internals/transactions/TransactionsWriter.hpp>
#include "internals/transactions/Uint16IndexedTransactionsList.hpp"
#include "internals/transactions/Uint32IndexedTransactionsList.hpp"
#include "internals/transactions/Uint8IndexedTransactionsList.hpp"

namespace internals {
namespace transactions {


TransactionsList::TransactionsList() {
}


TransactionsList::~TransactionsList() {
}

unique_ptr<TransactionsList> TransactionsList::newEmptyTransactionList(
		Counters* counters, int32_t& max_tid) {

	TransactionsList *trnlist;
	if (Uint8IndexedTransactionsList::compatible(counters)) {
		trnlist = new Uint8IndexedTransactionsList(counters);
		max_tid = Uint8IndexedTransactionsList::getMaxTransId(counters);
	} else if (Uint16IndexedTransactionsList::compatible(counters)) {
		trnlist = new Uint16IndexedTransactionsList(counters);
		max_tid = Uint16IndexedTransactionsList::getMaxTransId(counters);
	} else {
		trnlist = new Uint32IndexedTransactionsList(counters);
		max_tid = Uint32IndexedTransactionsList::getMaxTransId(counters);
	}
	return unique_ptr<TransactionsList >(trnlist);
}

// TransactionsSubList class
// --------------------------
TransactionsSubList::TransactionsSubList(TransactionsList *transactions,
		unique_ptr<TidList::ItemTidList> tidList) {
	_tids = std::move(tidList);
	_transactions = transactions;
}

void TransactionsSubList::count(int32_t& transactionsCount,
		int32_t& distinctTransactionsCount, int32_t* supportCounts,
		int32_t* distinctTransactionsCounts,
		int32_t extension, int32_t maxItem) {

	_transactions->countSubList(_tids.get(),
			transactionsCount,	distinctTransactionsCount,
			supportCounts, distinctTransactionsCounts, extension, maxItem);
}

void TransactionsSubList::copyTo(TransactionsWriter* writer, TidList* tidList,
		int32_t* renaming, int32_t coreItem) {
	_transactions->copyTo(_tids.get(), writer, tidList,
				renaming, coreItem);
}

}
}

