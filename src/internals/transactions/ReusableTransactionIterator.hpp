

#pragma once

#include <internals/transactions/TransactionIterator.hpp>

namespace internals {
namespace transactions {

struct ReusableTransactionIterator
    : public virtual TransactionIterator
{
    virtual void setTransaction(int32_t transaction) = 0;
};

}
}
