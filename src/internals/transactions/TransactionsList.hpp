
#pragma once

#include <memory>
using namespace std;
#include "internals/tidlist/TidList.hpp"
#include "util/Iterator.hpp"
#include "util/shortcuts.h"
using namespace util;
using internals::tidlist::TidList;

namespace internals {

class Counters;

namespace transactions {

/**
 * Stores transactions. Items in transactions are assumed to be sorted in
 * increasing order
 */

class TransactionsList
{
protected:
    TransactionsList();

public:
    virtual ~TransactionsList();
    virtual int32_t size() = 0;
    virtual void countSubList(TidList::ItemTidList *tidlist,
    		int32_t& transactionsCount, int32_t& distinctTransactionsCount,
    		int32_t* supportCounts, int32_t* distinctTransactionsCounts,
    		int32_t extension, int32_t maxItem) = 0;

};

}
}
