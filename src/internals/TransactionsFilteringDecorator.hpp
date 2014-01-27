

#pragma once

#include "internals/TransactionsIteratorDecorator.hpp"
#include "util/shortcuts.h"

using namespace util;

namespace internals {

class TransactionsFilteringDecorator : public TransactionsIteratorDecorator
{
protected:
	shp_array_int32 _support;
	int32_t *_support_fast;

public:
    int32_t transform(int32_t in) override;

    TransactionsFilteringDecorator(
    		Iterator<TransactionReader*>* filtered,
    		shp_array_int32 support);
};
}
