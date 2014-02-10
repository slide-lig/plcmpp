
#pragma once

#include <memory>
using namespace std;
#include "internals/transactions/TransactionsWriter.hpp"
#include "internals/tidlist/TidList.hpp"
#include "util/Iterator.hpp"
#include "util/shortcuts.h"
using namespace util;
using internals::tidlist::TidList;

namespace internals {

class Counters;

namespace transactions {

class CopyableTransactionsList
{
public:
	virtual ~CopyableTransactionsList() {}
    virtual void copyTo(TransactionsWriter *writer, TidList *tidList,
    			int32_t *renaming, int32_t coreItem) = 0;
};

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
    virtual unique_ptr<Iterator<int32_t>> getIdIterator() = 0;
    virtual unique_ptr<TransactionsWriter> getWriter() = 0;
    virtual int32_t size() = 0;
    virtual void compress() = 0;
    virtual void countSubList(TidList::ItemTidList *tidlist,
    		int32_t& transactionsCount, int32_t& distinctTransactionsCount,
    		int32_t* supportCounts, int32_t* distinctTransactionsCounts,
    		int32_t extension, int32_t maxItem) = 0;
    virtual void copyTo(TidList::ItemTidList* item_tidList,
    		TransactionsWriter* writer, TidList* new_tidList,
    		int32_t* renaming, int32_t coreItem) = 0;

    static unique_ptr<TransactionsList> newEmptyTransactionList(
			Counters *counters, int32_t &max_tid);
};

class TransactionsSubList : public CopyableTransactionsList
{
protected:
	unique_ptr<TidList::ItemTidList> _tids;
	TransactionsList *_transactions;

public:
    TransactionsSubList(TransactionsList* transactions,
    		unique_ptr<TidList::ItemTidList> tidList);
    void count(int32_t &transactionsCount, int32_t &distinctTransactionsCount,
    			int32_t *supportCounts, int32_t *distinctTransactionsCounts,
    			int32_t extension, int32_t maxItem);
    void copyTo(TransactionsWriter *writer, TidList *tidList,
    			int32_t *renaming, int32_t coreItem);

};

}
}
