

#pragma once

#include <vector>
using namespace std;

#include <internals/TransactionReader.hpp>

namespace internals	{
namespace transactions	{


struct TransactionIterator
    : public virtual TransactionReader
    , public virtual vector<int32_t>::iterator
{
    virtual void setTransactionSupport(int32_t s) = 0;
    virtual void remove() = 0;
};

}
}
