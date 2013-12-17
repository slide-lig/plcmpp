
#include <internals/TransactionsFilteringDecorator.hpp>

namespace internals {

TransactionsFilteringDecorator::TransactionsFilteringDecorator(
		Iterator<TransactionReader*>* filtered,
		shp_vec_int32 support) :
		TransactionsIteratorDecorator(filtered), _support(support) {
}

int32_t TransactionsFilteringDecorator::transform(int32_t in) {
	if ((*_support)[in] > 0)
		return in;
	else
		return -1;
}

}

