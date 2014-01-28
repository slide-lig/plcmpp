
#include <climits>
using namespace std;

#include "PLCM.hpp"
#include "internals/Dataset.hpp"
#include "internals/Counters.hpp"
#include "internals/TransactionReader.hpp"
#include "internals/transactions/TransactionsWriter.hpp"
#include "internals/transactions/Uint16IndexedTransactionsList.hpp"
#include "internals/transactions/Uint32IndexedTransactionsList.hpp"
#include "internals/transactions/Uint8IndexedTransactionsList.hpp"
#include "internals/tidlist/Uint16ConsecutiveItemsConcatenatedTidList.hpp"
#include "internals/tidlist/Uint32ConsecutiveItemsConcatenatedTidList.hpp"

namespace internals {

// Dataset class
// -------------

Dataset::Dataset(
		Counters* counters,
		Iterator<TransactionReader*>* transactions) :
				Dataset(counters, transactions, INT_MAX) {
}

Dataset::Dataset(Counters* counters,
		Iterator<TransactionReader*>* transactions, int32_t tidListBound) {
	int32_t maxTransId;

	TransactionsList *trnlist;
	if (Uint8IndexedTransactionsList::compatible(counters)) {
		trnlist = new Uint8IndexedTransactionsList(counters);
		maxTransId = Uint8IndexedTransactionsList::getMaxTransId(counters);
	} else if (Uint16IndexedTransactionsList::compatible(counters)) {
		trnlist = new Uint16IndexedTransactionsList(counters);
		maxTransId = Uint16IndexedTransactionsList::getMaxTransId(counters);
	} else {
		trnlist = new Uint32IndexedTransactionsList(counters);
		maxTransId = Uint32IndexedTransactionsList::getMaxTransId(counters);
	}
	_transactions = unique_ptr<TransactionsList>(trnlist);

	TidList *tidlist;
	if (Uint16ConsecutiveItemsConcatenatedTidList::compatible(maxTransId)) {
		tidlist = new Uint16ConsecutiveItemsConcatenatedTidList(
				counters, tidListBound);
	} else {
		tidlist = new Uint32ConsecutiveItemsConcatenatedTidList(
				counters, tidListBound);
	}
	_tidList = unique_ptr<TidList>(tidlist);

	unique_ptr<TransactionsWriter> writer = _transactions->getWriter();
	while (transactions->hasNext()) {
		TransactionReader* transaction = transactions->next();
		if (transaction->getTransactionSupport() != 0 &&
				transaction->hasNext()) {
			int32_t transId = writer->beginTransaction(
					transaction->getTransactionSupport());

			while (transaction->hasNext()) {
				int32_t item = transaction->next();
				writer->addItem(item);

				if (item < tidListBound) {
					_tidList->addTransaction(item, transId);
				}
			}

			writer->endTransaction();
		}
	}
}

Dataset::Dataset(const Dataset& other) {
	_transactions = other._transactions->clone();
	_tidList = other._tidList->clone();
}

unique_ptr<Dataset> Dataset::clone() {
	return unique_ptr<Dataset>(
			new Dataset(*this));
}

void Dataset::compress(int32_t coreItem) {
	PLCM::getCurrentThread()->counters[
	         PLCM::PLCMCounters::TransactionsCompressions]++;
	_transactions->compress(coreItem);
}

unique_ptr<TransactionsIterable> Dataset::getSupport(int32_t item) {
	return unique_ptr<TransactionsIterable>(
			new TransactionsIterable(this, _tidList->getIterable(item)));
}

unique_ptr<ReusableTransactionIterator> Dataset::getTransactionIterator() {
	return _transactions->getIterator();
}

unique_ptr<Iterator<int32_t> > Dataset::getTidList(int32_t item) {
	return _tidList->get(item);
}

int32_t Dataset::getStoredTransactionsCount() {
	return _transactions->size();
}

// TransactionsIterable class
// --------------------------

TransactionsIterable::TransactionsIterable(Dataset* dataset,
		unique_ptr<TidList::TIntIterable> tidList) {
	_tids = std::move(tidList);
	_dataset = dataset;
}

unique_ptr<Iterator<TransactionReader*> > TransactionsIterable::iterator() {
	return unique_ptr<Iterator<TransactionReader*> >(
			new TransactionsIterator(_dataset, _tids->iterator()));
}

// TransactionsIterator class
// --------------------------

TransactionsIterator::TransactionsIterator(Dataset* dataset,
		unique_ptr<Iterator<int32_t> > tids) {
	_it = std::move(tids);
	_transIter = dataset->getTransactionIterator();
}

TransactionReader* TransactionsIterator::next() {
	_transIter->setTransaction(_it->next());
	return _transIter.get();
}

bool TransactionsIterator::hasNext() {
	return _it->hasNext();
}

}
