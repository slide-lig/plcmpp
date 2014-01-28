#pragma once

#include <internals/transactions/IndexedTransactionsList.hpp>
#include <util/RawArray.hpp>

using util::RawArray;

namespace internals {

class Counters;

namespace transactions {

template <class T, class IteratorT>
class Template_IndexedReusableIterator;

template <class T>
class Template_IndexedTransactionsList: public IndexedTransactionsList {

private:
	typedef Template_ReusableTransactionIterator<T> NativeIterator;
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

	template <class IteratorT>
	unique_ptr<Template_ReusableTransactionIterator<IteratorT> > getIteratorWithType();
	template <class IteratorT>
	void positionIterator(int32_t transaction,
			Template_IndexedReusableIterator<T, IteratorT> *iter);

	unique_ptr<ReusableTransactionIterator> getIterator() override;
	unique_ptr<TransactionsList> clone() override;
    void compress(int32_t prefixEnd) override;

	static bool compatible(Counters* c);
	static int32_t getMaxTransId(Counters* c);

private:
    static void sort(array_int32 &array, int32_t start, int32_t end,
    		NativeIterator *it1,
    		NativeIterator *it2,
    		T prefixEnd);
    static int32_t merge(
    		NativeIterator *it1,
    		NativeIterator *it2,
    		T prefixEnd);

protected:
	void writeItem(int32_t item) override;
};

template <class T, class IteratorT>
class Template_IndexedReusableIterator: public Template_ReusableTransactionIterator<IteratorT> {
private:
	int transNum;
	Template_IndexedTransactionsList<T> *_tlist;

public:
	Template_IndexedReusableIterator(Template_IndexedTransactionsList<T> *tlist) {
		_tlist = tlist;
		transNum = 0;
	}
	virtual void set(int32_t begin, int32_t end) = 0;
	void setTransaction(int32_t transaction)  {
		transNum = transaction;
		_tlist->positionIterator(transaction, this);
	}
	int32_t getTransactionSupport(){
		return _tlist->getTransSupport(transNum);
	}
	void setTransactionSupport(int32_t s){
		_tlist->setTransSupport(transNum, s);
	}
};

template <class T, class IteratorT>
class Template_TransIter: public Template_IndexedReusableIterator<T, IteratorT>
{
protected:
    T* _pos;
    T* _nextPos;
	T* _concatenated;
	T* _end;
    bool _first;

protected:
    void set(int32_t begin, int32_t end) override {
    	_nextPos = _concatenated + begin - 1;
    	_end = _concatenated + end;
    	_first = true;
    }

private:
    void findNext() {
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
    Template_TransIter(
    		Template_IndexedTransactionsList<T> *tlist,
			T* concatenated_fast) :
				Template_IndexedReusableIterator<T, IteratorT>(tlist) {
    	_concatenated = concatenated_fast;
    	_first = true;
    	_end = _pos = _nextPos = _concatenated;
    }

    IteratorT next() override {
    	_pos = _nextPos;
    	findNext();
    	return *_pos;
    }

    bool hasNext() override {
    	if (_first) {
    		_first = false;
    		findNext();
    	}
    	return _nextPos != nullptr;
    }

    void remove() override {
    	*_pos = Template_IndexedTransactionsList<T>::MAX_VALUE;
    }
};

}
}

/* Template classes, we need their definition */
#include <internals/transactions/Template_IndexedTransactionsList_impl.hpp>
