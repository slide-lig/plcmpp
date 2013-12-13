
#include <internals/Dataset.hpp>
#include "internals/Counters.hpp"
#include <internals/TransactionReader.hpp>
#include <internals/transactions/TransactionsWriter.hpp>

namespace internals {

void Dataset::compress(int32_t coreItem) {
}

unique_ptr<Dataset> Dataset::clone() {
}

unique_ptr<TransactionsIterable> Dataset::getSupport(int32_t item) {
}

Dataset::Dataset(Counters* counters,
		Iterator<TransactionReader*>* transactions) {
}

Dataset::Dataset(Counters* counters,
		Iterator<TransactionReader*>* transactions, int32_t tidListBound) {
}

Dataset::~Dataset() {
}

Dataset::Dataset(TransactionsList* transactions,
		TidList* occurrences) {
}

int32_t Dataset::getStoredTransactionsCount() {
}

TransactionsIterable::TransactionsIterable(Dataset* dataset,
		unique_ptr<TidList::TIntIterable> tidList) {
}

unique_ptr<Iterator<TransactionReader*> > TransactionsIterable::iterator() {
}

TransactionsIterator::TransactionsIterator(Dataset* dataset,
		unique_ptr<Iterator<int32_t> > tids) {
}

TransactionReader* TransactionsIterator::next() {
}

bool TransactionsIterator::hasNext() {
}

}
