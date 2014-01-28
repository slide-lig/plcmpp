#pragma once

#include <internals/transactions/IndexedTransactionsList.hpp>
#include <util/RawArray.hpp>

using util::RawArray;

namespace internals {

class Counters;

namespace transactions {

template <class T>
class Template_IndexedTransactionsList: public IndexedTransactionsList {

private:
	RawArray<T>* _concatenated;
	T* _concatenated_fast;

public:
	static const T MAX_VALUE;

	Template_IndexedTransactionsList(Counters* c);
	Template_IndexedTransactionsList(int32_t transactionsLength,
			int32_t nbTransactions);
	Template_IndexedTransactionsList(
			const Template_IndexedTransactionsList& other);
	~Template_IndexedTransactionsList();

	unique_ptr<ReusableTransactionIterator> getIterator() override;
	unique_ptr<TransactionsList> clone() override;

	static bool compatible(Counters* c);
	static int32_t getMaxTransId(Counters* c);

protected:
	void writeItem(int32_t item) override;
};

template <class T>
class Template_TransIter: public IndexedReusableIterator
{
protected:
    T* _pos;
    T* _nextPos;
	T* _concatenated;
	T* _end;
    bool _first;

protected:
    inline void set(int32_t begin, int32_t end) override {
    	_nextPos = _concatenated + begin - 1;
    	_end = _concatenated + end;
    	_first = true;
    }

private:
    inline void findNext() {
    	while (true) {
    		++_nextPos;
    		if (_nextPos == _end) {
    			_nextPos = nullptr;
    			return;
    		}
    		if (*_nextPos !=
        			Template_IndexedTransactionsList<T>::MAX_VALUE) {
    			return;
    		}
    	}
    }

public:
    inline Template_TransIter(
			IndexedTransactionsList *tlist,
			T* concatenated_fast) :
				IndexedReusableIterator(tlist) {
    	_concatenated = concatenated_fast;
    	_first = true;
    	_end = _pos = _nextPos = _concatenated;
    }

    inline int32_t next() override {
    	_pos = _nextPos;
    	findNext();
    	return *_pos;
    }

    inline bool hasNext() override {
    	if (_first) {
    		_first = false;
    		findNext();
    	}
    	return _nextPos != nullptr;
    }

    inline void remove() override {
    	*_pos = Template_IndexedTransactionsList<T>::MAX_VALUE;
    }
};

}
}

/* Template classes, we need their definition */
#include <internals/transactions/Template_IndexedTransactionsList_impl.hpp>
