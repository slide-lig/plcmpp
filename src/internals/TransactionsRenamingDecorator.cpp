
#include <internals/TransactionsRenamingDecorator.hpp>

namespace internals {

TransactionsRenamingDecorator::TransactionsRenamingDecorator(
		Iterator<TransactionReader*>* filtered,
		shp_array_int32 rebasing) :
		TransactionsIteratorDecorator(filtered), _rebasing(rebasing) {
	_rebasing_fast = rebasing->array;
}

int32_t TransactionsRenamingDecorator::transform(int32_t in) {
	return _rebasing_fast[in];
}

}

