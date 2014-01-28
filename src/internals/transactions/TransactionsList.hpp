
#pragma once

#include <memory>
using namespace std;
#include "internals/transactions/ReusableTransactionIterator.hpp"
#include "internals/transactions/TransactionsWriter.hpp"
#include "util/Iterator.hpp"
#include "util/shortcuts.h"
using namespace util;

namespace internals {
namespace transactions {

/**
 * Stores transactions. Items in transactions are assumed to be sorted in
 * increasing order
 */

class TransactionsList
{
public:
    TransactionsList();
    virtual ~TransactionsList();

    virtual unique_ptr<TransactionsList> clone() = 0;
    virtual unique_ptr<ReusableTransactionIterator> getIterator() = 0;
    virtual unique_ptr<Iterator<int32_t>> getIdIterator() = 0;
    virtual unique_ptr<TransactionsWriter> getWriter() = 0;
    virtual int32_t size() = 0;
    virtual void compress(int32_t prefixEnd) = 0;
};

}
}
