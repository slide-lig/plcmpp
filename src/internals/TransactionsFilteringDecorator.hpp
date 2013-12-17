

#pragma once

#include <vector>
using namespace std;

#include "internals/TransactionsIteratorDecorator.hpp"
#include "util/shortcuts.h"

using namespace util;

namespace internals {

class TransactionsFilteringDecorator : public TransactionsIteratorDecorator
{
protected:
	shp_vec_int32 _support;

public:
    int32_t transform(int32_t in) override;

    TransactionsFilteringDecorator(
    		Iterator<TransactionReader*>* filtered,
    		shp_vec_int32 support);
};
}
