
#include <internals/TransactionsIteratorDecorator.hpp>
#include <util/Iterator.hpp>

namespace internals {

void TransactionsIteratorDecorator::FilteredTransaction::reset(
		TransactionReader* filtered) {
}

void TransactionsIteratorDecorator::FilteredTransaction::findNext() {
}

int32_t TransactionsIteratorDecorator::FilteredTransaction::getTransactionSupport() {
}

int32_t TransactionsIteratorDecorator::FilteredTransaction::next() {
}

bool TransactionsIteratorDecorator::FilteredTransaction::hasNext() {
}

TransactionsIteratorDecorator::FilteredTransaction::FilteredTransaction(
		TransactionsIteratorDecorator* deco, TransactionReader* filtered) {
}

bool TransactionsIteratorDecorator::hasNext() {
}

TransactionReader* TransactionsIteratorDecorator::next() {
}

TransactionsIteratorDecorator::TransactionsIteratorDecorator(
		Iterator<TransactionReader*>* filtered) {
}

}

