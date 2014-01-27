

#pragma once

#include <internals/TransactionReader.hpp>

namespace internals {
namespace transactions {

struct ReusableTransactionIterator
    : public TransactionReader
{
    virtual void setTransaction(int32_t transaction) = 0;
    virtual void setTransactionSupport(int32_t s) = 0;
    virtual void remove() = 0;
};

}
}
