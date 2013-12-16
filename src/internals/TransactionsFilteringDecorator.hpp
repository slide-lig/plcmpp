

#pragma once

#include <vector>
using namespace std;

#include <internals/TransactionsIteratorDecorator.hpp>

using namespace util;

namespace internals {

class TransactionsFilteringDecorator : public TransactionsIteratorDecorator
{
protected:
    vector<int32_t>* _support;

public:
    int32_t transform(int32_t in) override;

    TransactionsFilteringDecorator(
    		Iterator<TransactionReader*>* filtered,
    		vector<int32_t>* support);
};
}
