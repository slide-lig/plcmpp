
#pragma once

#include <memory>
using namespace std;

#include "internals/transactions/TransactionsList.hpp"
#include "internals/tidlist/TidList.hpp"
#include "util/Iterator.hpp"
#include "util/shortcuts.h"

using namespace internals::transactions;
using namespace internals::tidlist;
using namespace util;

namespace internals {

class TransactionsIterable;
class TransactionReader;
class Counters;

/**
 * Stores transactions and does occurrence delivery
 */
class Dataset
{
protected:
	unique_ptr<TransactionsList> transactions;

	/**
	 * frequent item => array of occurrences indexes in "concatenated"
	 * Transactions are added in the same order in all occurrences-arrays.
	 */
	unique_ptr<TidList> tidLists;

public:
    void compress(int32_t coreItem);
    unique_ptr<Dataset> clone();
    unique_ptr<TransactionsIterable> getSupport(
    						int32_t item);
    Dataset(Counters* counters, Iterator<TransactionReader*>* transactions);
    /**
	 * @param counters
	 * @param transactions
	 *            assumed to be filtered according to counters
	 * @param tidListBound
	 *            - highest item (exclusive) which will have a tidList. set to
	 *            MAX_VALUE when using predictive pptest.
	 */
    Dataset(Counters* counters, Iterator<TransactionReader*>* transactions, int32_t tidListBound);
    ~Dataset();

protected:
    Dataset(TransactionsList* transactions, TidList* occurrences);
	/**
	 * @return how many transactions (ignoring their weight) are stored behind
	 *         this dataset
	 */

    int32_t getStoredTransactionsCount();
};

class TransactionsIterable
{
protected:
	unique_ptr<TidList::TIntIterable> tids;

public:
    TransactionsIterable(Dataset *dataset,
    		unique_ptr<TidList::TIntIterable> tidList);
    unique_ptr<Iterator<TransactionReader*> > iterator();
};

class TransactionsIterator : public Iterator<TransactionReader*>
{
protected:
	unique_ptr<Iterator<int32_t> > it;
	unique_ptr<ReusableTransactionIterator> transIter;

public:
    TransactionsIterator(Dataset *dataset,
    		unique_ptr<Iterator<int32_t> > tids);
    TransactionReader* next() override;
    bool hasNext() override;
};


}
