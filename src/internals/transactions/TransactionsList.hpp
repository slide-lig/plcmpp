
#pragma once

#include <vector>
#include <memory>
using namespace std;
#include "internals/transactions/ReusableTransactionIterator.hpp"
#include "internals/transactions/TransactionsWriter.hpp"
#include "util/Iterator.hpp"
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
    virtual void compress(int32_t prefixEnd);

private:
    static void sort(vector<int32_t>* array, int32_t start, int32_t end,
    		ReusableTransactionIterator *it1,
    		ReusableTransactionIterator *it2,
    		int32_t prefixEnd);
    static int32_t merge(
    		ReusableTransactionIterator *it1,
    		ReusableTransactionIterator *it2,
    		int32_t prefixEnd);
};

}
}
