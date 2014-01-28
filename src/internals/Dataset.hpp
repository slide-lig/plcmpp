
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
class Counters;

/**
 * Stores transactions and does occurrence delivery
 */
class Dataset
{
protected:
	unique_ptr<TransactionsList> _transactions;

	/**
	 * frequent item => array of occurrences indexes in "concatenated"
	 * Transactions are added in the same order in all occurrences-arrays.
	 */
	unique_ptr<TidList> _tidList;

public:
    void compress(int32_t coreItem);
    unique_ptr<Dataset> clone();
    unique_ptr<TransactionsIterable> getSupport(
    						int32_t item);
    unique_ptr<ReusableTransactionIterator> getTransactionIterator();
    unique_ptr<Iterator<int32_t> > getTidList(int32_t item);

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

protected:
    Dataset(const Dataset& other);
	/**
	 * @return how many transactions (ignoring their weight) are stored behind
	 *         this dataset
	 */

    int32_t getStoredTransactionsCount();
};

class TransactionsIterable
{
protected:
	unique_ptr<TidList::TIntIterable> _tids;
	Dataset *_dataset;

public:
    TransactionsIterable(Dataset *dataset,
    		unique_ptr<TidList::TIntIterable> tidList);
    unique_ptr<Iterator<TransactionReader*> > iterator();
};

class TransactionsIterator : public Iterator<TransactionReader*>
{
protected:
	unique_ptr<Iterator<int32_t> > _it;
	unique_ptr<ReusableTransactionIterator> _transIter;

public:
    TransactionsIterator(Dataset *dataset,
    		unique_ptr<Iterator<int32_t> > tids);
    TransactionReader* next() override;
    bool hasNext() override;
};


}
