
#include <internals/TransactionsIteratorDecorator.hpp>
#include <util/Iterator.hpp>

namespace internals {

// class TransactionsIteratorDecorator
// -----------------------------------
TransactionsIteratorDecorator::TransactionsIteratorDecorator(
		Iterator<TransactionReader*>* filtered) {
	_wrapped = filtered;
	_instance = nullptr;
}

bool TransactionsIteratorDecorator::hasNext() {
	return _wrapped->hasNext();
}

TransactionReader* TransactionsIteratorDecorator::next() {
	if (_instance == nullptr) {
		_instance = unique_ptr<FilteredTransaction>(
				new FilteredTransaction(this, _wrapped->next()));
	} else {
		_instance->setTransationReader(_wrapped->next());
	}

	return _instance.get();
}


// class FilteredTransaction
// -------------------------
TransactionsIteratorDecorator::FilteredTransaction::FilteredTransaction(
		TransactionsIteratorDecorator* deco, TransactionReader* filtered) {
	_deco = deco;
	setTransationReader(filtered);
}


void TransactionsIteratorDecorator::FilteredTransaction::setTransationReader(
		TransactionReader* filtered) {
	_wrapped = filtered;
	_next = 0;

	findNext();
}

void TransactionsIteratorDecorator::FilteredTransaction::findNext() {
	while (_wrapped->hasNext()) {
		_next = _deco->transform(_wrapped->next());
		if (_next != -1 ) {
			_hasNext = true;
			return;
		}
	}

	_hasNext = false;
}

int32_t TransactionsIteratorDecorator::FilteredTransaction::getTransactionSupport() {
	return _wrapped->getTransactionSupport();
}

int32_t TransactionsIteratorDecorator::FilteredTransaction::next() {
	int32_t value = _next;
	findNext();
	return value;
}

bool TransactionsIteratorDecorator::FilteredTransaction::hasNext() {
	return _hasNext;
}

}

