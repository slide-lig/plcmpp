
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

namespace transactions {
class TransactionsSubList;
}
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
    void compress(int32_t max_candidate);
    unique_ptr<TransactionsSubList> getTransactionsSubList(int32_t item);
    unique_ptr<Iterator<int32_t> > getItemTidListIterator(int32_t item);

     /**
	 * @param counters
	 * @param transactions
	 *            assumed to be filtered according to counters
	 * @param tidListBound
	 *            - highest item (exclusive) which will have a tidList. set to
	 *            MAX_VALUE when using predictive pptest.
	 */
    Dataset(Counters* counters, CopyableTransactionsList* item_transactions,
    		shp_array_int32 renaming, int32_t max_candidate);

protected:
	/**
	 * @return how many transactions (ignoring their weight) are stored behind
	 *         this dataset
	 */

    int32_t getStoredTransactionsCount();
};

}
