

#include "PLCM.hpp"
#include "internals/Dataset.hpp"
#include "internals/Counters.hpp"
#include "internals/TransactionReader.hpp"
#include "internals/transactions/TransactionsWriter.hpp"

namespace internals {

// Dataset class
// -------------

Dataset::Dataset(
		Counters* counters,
		CopyableTransactionsList* item_transactions,
		shp_array_int32 renaming,
		int32_t max_candidate) {

	int32_t maxTransId;

	_transactions = TransactionsList::newEmptyTransactionList(
							counters, maxTransId);
	_tidList = TidList::newEmptyTidList(counters, maxTransId);

	unique_ptr<TransactionsWriter> writer = _transactions->getWriter();

	item_transactions->copyTo(writer.get(), _tidList.get(),
			renaming->array, max_candidate);
}


void Dataset::compress(int32_t max_candidate) {
	//cout << max_candidate << endl;
	PLCM::getCurrentThread()->counters[
	         PLCM::PLCMCounters::TransactionsCompressions]++;
	_transactions->compress();
}


unique_ptr<TransactionsSubList > Dataset::getTransactionsSubList(int32_t item) {
	return unique_ptr<TransactionsSubList >(
			new TransactionsSubList(
					_transactions.get(), _tidList->getItemTidList(item)));
}


unique_ptr<Iterator<int32_t> > Dataset::getItemTidListIterator(int32_t item) {
	return _tidList->getItemTidList(item)->iterator();
}


int32_t Dataset::getStoredTransactionsCount() {
	return _transactions->size();
}

}
