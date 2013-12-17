

#pragma once

#include <vector>
using namespace std;

#include "internals/TransactionsIteratorDecorator.hpp"
#include "util/shortcuts.h"

using namespace util;

namespace internals {

class TransactionsRenamingDecorator : public TransactionsIteratorDecorator
{
protected:
	shp_vec_int32 _rebasing;

public:
    int32_t transform(int32_t in) override;

    TransactionsRenamingDecorator(
    		Iterator<TransactionReader*>* filtered,
    		shp_vec_int32 rebasing);
};
}
