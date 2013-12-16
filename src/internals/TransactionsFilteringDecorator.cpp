
#include <internals/TransactionsFilteringDecorator.hpp>

namespace internals {

TransactionsFilteringDecorator::TransactionsFilteringDecorator(
		Iterator<TransactionReader*>* filtered,
		vector<int32_t>* support) :
		TransactionsIteratorDecorator(filtered), _support(support) {
}

int32_t TransactionsFilteringDecorator::transform(int32_t in) {
	if ((*_support)[in] > 0)
		return in;
	else
		return -1;
}

}

