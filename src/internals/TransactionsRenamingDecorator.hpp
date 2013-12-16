

#pragma once

#include <vector>
using namespace std;

#include <internals/TransactionsIteratorDecorator.hpp>
using namespace util;

namespace internals {

class TransactionsRenamingDecorator : public TransactionsIteratorDecorator
{
protected:
    vector<int32_t>* _rebasing;

public:
    int32_t transform(int32_t in) override;

    TransactionsRenamingDecorator(
    		Iterator<TransactionReader*>* filtered,
    		vector<int32_t>* rebasing);
};
}
