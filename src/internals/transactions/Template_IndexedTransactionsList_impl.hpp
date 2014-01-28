
#include <iostream>
using namespace std;

#include <internals/transactions/Template_IndexedTransactionsList.hpp>
#include <internals/Counters.hpp>

namespace internals {
namespace transactions {

template <class T>
const T Template_IndexedTransactionsList<T>::MAX_VALUE = ~((T)0);

template <class T>
Template_IndexedTransactionsList<T>::Template_IndexedTransactionsList(
		Counters* c) : Template_IndexedTransactionsList<T>(
				c->distinctTransactionLengthSum,
				c->distinctTransactionsCount)
{
}

template <class T>
Template_IndexedTransactionsList<T>::Template_IndexedTransactionsList(
		int32_t transactionsLength, int32_t nbTransactions) :
		IndexedTransactionsList(nbTransactions)
{
	_concatenated = new RawArray<T>(transactionsLength);
	_concatenated_fast = _concatenated->array;
}

template <class T>
Template_IndexedTransactionsList<T>::Template_IndexedTransactionsList(
		const Template_IndexedTransactionsList<T>& other) :
				IndexedTransactionsList(other) {
	_concatenated = new RawArray<T>(*(other._concatenated));
	_concatenated_fast = _concatenated->array;
}

template <class T>
Template_IndexedTransactionsList<T>::~Template_IndexedTransactionsList()
{
	delete _concatenated;
}

template <class T>
template <class IteratorT>
unique_ptr<Template_ReusableTransactionIterator<IteratorT> > Template_IndexedTransactionsList<T>::getIteratorWithType() {
	return unique_ptr<Template_ReusableTransactionIterator<IteratorT> >(
			new Template_TransIter<T, IteratorT>(this, _concatenated_fast));
}

template <class T>
template <class IteratorT>
void Template_IndexedTransactionsList<T>::positionIterator(
		int32_t transaction,
		Template_IndexedReusableIterator<T, IteratorT>* iter)
{
	uint32_t startPos = transaction << 1;
	if (startPos >= _indexAndFreqs_size ||
			_indexAndFreqs_fast[startPos] == -1) {
		cerr << "transaction " << transaction <<
				" does not exist! Aborting." << endl;
		abort();
	} else {
		uint32_t endPos = startPos + 2;
		int32_t end;
		if (endPos < _indexAndFreqs_size) {
			end = _indexAndFreqs_fast[endPos];
			if (end == -1) {
				end = writeIndex;
			}
		} else {
			end = writeIndex;
		}
		iter->set(_indexAndFreqs_fast[startPos], end);
	}
}

template <class T>
unique_ptr<ReusableTransactionIterator> Template_IndexedTransactionsList<T>::getIterator() {
	return getIteratorWithType<int32_t>();
}

template <class T>
unique_ptr<TransactionsList> Template_IndexedTransactionsList<T>::clone() {
	// create a clone using the copy constructor
	Template_IndexedTransactionsList<T> *cloned = new
			Template_IndexedTransactionsList<T>(*this);
	return unique_ptr<TransactionsList>(cloned);
}

template <class T>
bool Template_IndexedTransactionsList<T>::compatible(
		Counters* c) {
	return c->getMaxFrequent() < MAX_VALUE;
}

template <class T>
int32_t Template_IndexedTransactionsList<T>::getMaxTransId(
		Counters* c) {
	return c->distinctTransactionsCount - 1;
}

template<class T>
inline void Template_IndexedTransactionsList<T>::compress(int32_t prefixEnd) {
	array_int32 sortList(size());
	unique_ptr<Iterator<int32_t> > idIter = getIdIterator();
	auto end = sortList.end();
	auto begin = sortList.begin();
	for (auto it = begin; it != end; it++) {
		*it = idIter->next();
	}
	unique_ptr<NativeIterator> it1 = getIteratorWithType<T>();
	unique_ptr<NativeIterator> it2 = getIteratorWithType<T>();
	sort(begin, end, it1.get(), it2.get(), prefixEnd);
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
inline void Template_IndexedTransactionsList<T>::sort(
		int32_t* start, int32_t* end, NativeIterator* it1,
		NativeIterator* it2, int32_t prefixEnd) {
	if (start >= end - 1) {
		// size 0 or 1
		return;
	} else if (end - start == 2) {
		it1->setTransaction(*start);
		it2->setTransaction(*(start + 1));
		merge(it1, it2, prefixEnd);
	} else {
		// pick pivot at the middle and put it at the end
		int32_t* pivotPos = start + ((end - start) / 2);
		int32_t pivotVal = *pivotPos;
		*pivotPos = *(end - 1);
		*(end - 1) = pivotVal;
		int32_t* insertInf = start;
		int32_t* insertSup = end - 2;
		for (int32_t* i = start; i <= insertSup;) {
			it1->setTransaction(pivotVal);
			it2->setTransaction(*i);
			int32_t comp = merge(it1, it2, prefixEnd);
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
		sort(start, insertInf, it1, it2, prefixEnd);
		sort(insertSup + 2, end, it1, it2, prefixEnd);
	}
}

template<class T>
inline int32_t Template_IndexedTransactionsList<T>::merge(
		NativeIterator* t1, NativeIterator* t2,
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
	int32_t t1Item = t1->next();
	int32_t t2Item = t2->next();
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
								t1->removeAllNextItems();
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
					t1->removeAllNextItems();
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
					t1->removeAllNextItems();
					t1->setTransactionSupport(t1->getTransactionSupport() + t2->getTransactionSupport());
					t2->setTransactionSupport(0);
					return 0;
				}
			}
		}
	}
	return 0;
}

template <class T>
void Template_IndexedTransactionsList<T>::writeItem(
		int32_t item) {
	if (item >= MAX_VALUE) {
		cerr << item <<
				" too big for this kind of transaction list! Aborting."
				<< endl;
		abort();
	}
	_concatenated_fast[writeIndex] = (T) item;
	writeIndex++;
}

}
}
