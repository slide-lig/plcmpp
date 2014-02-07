
#include <iostream>
using namespace std;

#include <internals/transactions/IndexedTransactionsList.hpp>
#include <internals/Counters.hpp>
#include <util/SimpleDigest.h>

namespace internals {
namespace transactions {

template<class T>
inline void IndexedTransactionsList<T>::compress() {
	array_int32 sortList(size());
	unique_ptr<Iterator<int32_t> > idIter = getIdIterator();
	auto end = sortList.end();
	auto begin = sortList.begin();
	for (auto it = begin; it != end; it++) {
		*it = idIter->next();
	}
	sort(_transactions_info, begin, end);
}

/**
 * This is NOT a standard quicksort. Transactions with same prefix as the
 * pivot are left out the rest of the sort because they have been merged in
 * the pivot. Consequence: in the recursion, there is some space between
 * left sublist and right sublist (besides the pivot itself).
 *
 * @param array
 * @param start
 * @param end
 * @param it1
 * @param it2
 * @param prefixEnd
 */
template<class T>
inline void IndexedTransactionsList<T>::sort(
		descTransaction* transactions_info,
		int32_t* start, int32_t* end) {
	int32_t t1;
	int32_t t2;
	if (start >= end - 1) {
		// size 0 or 1
		return;
	} else if (end - start == 2) {
		t1 = *start;
		t2 = *(start + 1);
		merge(transactions_info, t1, t2);
	} else {
		// pick pivot at the middle and put it at the end
		int32_t* pivotPos = start + ((end - start) / 2);
		int32_t pivotVal = *pivotPos;
		*pivotPos = *(end - 1);
		*(end - 1) = pivotVal;
		int32_t* insertInf = start;
		int32_t* insertSup = end - 2;
		for (int32_t* i = start; i <= insertSup;) {
			t1 = pivotVal;
			t2 = *i;
			int32_t comp = merge(transactions_info, t1, t2);
			if (comp < 0) {
				int32_t valI = *i;
				*insertInf = valI;
				insertInf++;
				i++;
			} else if (comp > 0) {
				int32_t valI = *i;
				*i = *insertSup;
				*insertSup = valI;
				insertSup--;
			} else {
				i++;
			}
		}
		*(end - 1) = *(insertSup + 1);
		// Arrays.fill(array, insertInf, insertSup + 2, -1);
		*(insertSup + 1) = pivotVal;
		sort(transactions_info, start, insertInf);
		sort(transactions_info, insertSup + 2, end);
	}
}

template<class T>
inline int32_t IndexedTransactionsList<T>::merge(
		descTransaction* transactions_info,
		int32_t t1, int32_t t2) {

	descTransaction& info_t1 = transactions_info[t1];
	descTransaction& info_t2 = transactions_info[t2];

	// compare prefix hash (caution, hashes are unsigned integers,
	// a difference is not adequate)
	if (info_t1.prefix_hash < info_t2.prefix_hash)
		return 1;
	if (info_t2.prefix_hash < info_t1.prefix_hash)
		return -1;

	// compare prefix length
	int cmp = (info_t2.end_prefix - info_t2.start_transaction) -
			(info_t1.end_prefix - info_t1.start_transaction);

	if (cmp != 0)
		return cmp;

	// compare prefix
	cmp = std::memcmp(info_t1.start_transaction, info_t2.start_transaction,
				(info_t1.end_prefix - info_t1.start_transaction)*sizeof(T));

	if (cmp != 0)
	{
		ADD_DIGEST_FALSE_POSITIVE(info_t1.start_transaction, info_t1.end_prefix,
				info_t2.start_transaction, info_t2.end_prefix);
		return cmp;
	}

	// same prefix, we should merge
	info_t1.end_transaction = std::set_intersection(
			info_t1.end_prefix, info_t1.end_transaction,
			info_t2.end_prefix, info_t2.end_transaction,
			info_t1.end_prefix);

	info_t1.support += info_t2.support;
	info_t2.support = 0;

	return 0;
}

}
}
