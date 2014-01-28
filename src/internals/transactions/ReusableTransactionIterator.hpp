

#pragma once

#include <internals/TransactionReader.hpp>

namespace internals {
namespace transactions {

template <class T>
struct Template_ReusableTransactionIterator
    : public Template_TransactionReader<T>
{
    virtual void setTransaction(int32_t transaction) = 0;
    virtual void setTransactionSupport(int32_t s) = 0;
    virtual void remove() = 0;
    virtual void removeAllNextItems() = 0;
};

typedef Template_ReusableTransactionIterator<int32_t> ReusableTransactionIterator;
}
}
