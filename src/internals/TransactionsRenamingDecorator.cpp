
#include <internals/TransactionsRenamingDecorator.hpp>

namespace internals {

TransactionsRenamingDecorator::TransactionsRenamingDecorator(
		Iterator<TransactionReader*>* filtered,
		vector<int32_t>* rebasing) :
		TransactionsIteratorDecorator(filtered), _rebasing(rebasing) {
}

int32_t TransactionsRenamingDecorator::transform(int32_t in) {
	return (*_rebasing)[in];
}

}

