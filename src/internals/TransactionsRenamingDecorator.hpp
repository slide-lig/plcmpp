

#pragma once

#include "internals/TransactionsIteratorDecorator.hpp"
#include "util/shortcuts.h"

using namespace util;

namespace internals {

class TransactionsRenamingDecorator : public TransactionsIteratorDecorator
{
protected:
	shp_array_int32 _rebasing;
	int32_t *_rebasing_fast;

public:
    int32_t transform(int32_t in) override;

    TransactionsRenamingDecorator(
    		Iterator<TransactionReader*>* filtered,
    		shp_array_int32 rebasing);
};
}
