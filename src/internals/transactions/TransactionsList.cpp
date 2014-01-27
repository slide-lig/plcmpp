
#include <algorithm>
using namespace std;

#include <internals/transactions/TransactionsList.hpp>
#include <internals/transactions/ReusableTransactionIterator.hpp>
#include <internals/transactions/TransactionsWriter.hpp>

namespace internals {
namespace transactions {

TransactionsList::TransactionsList() {
}

TransactionsList::~TransactionsList() {
}

void TransactionsList::compress(int32_t prefixEnd) {
	array_int32 sortList(size());
	unique_ptr<Iterator<int32_t> > idIter = getIdIterator();
	auto end = sortList.end();
	for (auto it = sortList.begin(); it != end; it++) {
		*it = idIter->next();
	}
	unique_ptr<ReusableTransactionIterator> it1 = getIterator();
	unique_ptr<ReusableTransactionIterator> it2 = getIterator();
	sort(sortList, 0, sortList.size(), it1.get(), it2.get(), prefixEnd);
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
void TransactionsList::sort(array_int32 &array, int32_t start, int32_t end,
    		ReusableTransactionIterator *it1,
    		ReusableTransactionIterator *it2,
    		int32_t prefixEnd) {
	auto array_fast = array.array;
	if (start >= end - 1) {
		// size 0 or 1
		return;
	} else if (end - start == 2) {
		it1->setTransaction(array_fast[start]);
		it2->setTransaction(array_fast[start + 1]);
		merge(it1, it2, prefixEnd);
	} else {
		// pick pivot at the middle and put it at the end
		int pivotPos = start + ((end - start) / 2);
		int pivotVal = array_fast[pivotPos];
		array_fast[pivotPos] = array_fast[end - 1];
		array_fast[end - 1] = pivotVal;
		int insertInf = start;
		int insertSup = end - 2;
		for (int i = start; i <= insertSup;) {
			it1->setTransaction(pivotVal);
			it2->setTransaction(array_fast[i]);
			int comp = merge(it1, it2, prefixEnd);
			if (comp < 0) {
				int valI = array_fast[i];
				array_fast[insertInf] = valI;
				insertInf++;
				i++;
			} else if (comp > 0) {
				int valI = array_fast[i];
				array_fast[i] = array_fast[insertSup];
				array_fast[insertSup] = valI;
				insertSup--;
			} else {
				i++;
			}
		}
		array_fast[end - 1] = array_fast[insertSup + 1];
		// Arrays.fill(array, insertInf, insertSup + 2, -1);
		array_fast[insertSup + 1] = pivotVal;
		sort(array, start, insertInf, it1, it2, prefixEnd);
		sort(array, insertSup + 2, end, it1, it2, prefixEnd);
	}
}

int32_t TransactionsList::merge(
		ReusableTransactionIterator *t1,
		ReusableTransactionIterator *t2,
		int32_t prefixEnd) {
	if (!t1->hasNext()) {
		if (!t2->hasNext() || t2->next() > prefixEnd) {
			t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
			t2->setTransactionSupport(0);
			return 0;
		} else {
			return -1;
		}
	} else if (!t2->hasNext()) {
		if (t1->next() > prefixEnd) {
			t1->remove();
			while (t1->hasNext()) {
				t1->remove();
			}
			t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
			t2->setTransactionSupport(0);
			return 0;
		} else {
			return 1;
		}
	}
	int t1Item = t1->next();
	int t2Item = t2->next();
	while (true) {
		if (t1Item < prefixEnd) {
			if (t2Item < prefixEnd) {
				if (t1Item != t2Item) {
					return t1Item - t2Item;
				} else {
					if (t1->hasNext()) {
						t1Item = t1->next();
						if (t2->hasNext()) {
							t2Item = t2->next();
							continue;
						} else {
							if (t1Item < prefixEnd) {
								return 1;
							} else {
								t1->remove();
								while (t1->hasNext()) {
									t1Item = t1->next();
									t1->remove();
								}
								t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
								t2->setTransactionSupport(0);
								return 0;
							}
						}
					} else {
						if (t2->hasNext()) {
							t2Item = t2->next();
							if (t2Item < prefixEnd) {
								return -1;
							} else {
								t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
								t2->setTransactionSupport(0);
								return 0;
							}
						} else {
							t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
							t2->setTransactionSupport(0);
							return 0;
						}
					}
				}
			} else {
				return -1;
			}
		} else {
			if (t2Item < prefixEnd) {
				return 1;
			} else {
				break;
			}
		}
	}
	while (true) {
		if (t1Item == t2Item) {
			if (t1->hasNext()) {
				if (t2->hasNext()) {
					t1Item = t1->next();
					t2Item = t2->next();
					continue;
				} else {
					while (t1->hasNext()) {
						t1Item = t1->next();
						t1->remove();
					}
					t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
					t2->setTransactionSupport(0);
					return 0;
				}
			} else {
				t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
				t2->setTransactionSupport(0);
				return 0;
			}
		} else {
			if (t1Item < t2Item) {
				t1->remove();
				if (t1->hasNext()) {
					t1Item = t1->next();
				} else {
					t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
					t2->setTransactionSupport(0);
					return 0;
				}
			} else {
				if (t2->hasNext()) {
					t2Item = t2->next();
				} else {
					t1->remove();
					while (t1->hasNext()) {
						t1Item = t1->next();
						t1->remove();
					}
					t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
					t2->setTransactionSupport(0);
					return 0;
				}
			}
		}
	}

	return 0;
}

}
}
