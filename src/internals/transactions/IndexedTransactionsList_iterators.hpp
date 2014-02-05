#pragma once

#include <internals/transactions/IndexedTransactionsList.hpp>

namespace internals {

class Counters;

namespace transactions {

template <class T, class IteratorT>
class Template_IndexedReusableIterator: public Template_ReusableTransactionIterator<IteratorT> {
private:
	int transNum;
	IndexedTransactionsList<T> *_tlist;

public:
	Template_IndexedReusableIterator(IndexedTransactionsList<T> *tlist) {
		_tlist = tlist;
		transNum = 0;
	}
	virtual void set(T* begin, T* end) = 0;
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
	T* _begin;
	T* _end;

protected:
    void set(T* begin, T* end) override {
    	_begin = begin;
    	_end = end;
    }

public:
    Template_TransIter(IndexedTransactionsList<T> *tlist) :
				Template_IndexedReusableIterator<T, IteratorT>(tlist) {
    	set(nullptr, nullptr);
    }

    IteratorT next() override {
    	return *(_begin++);
    }

    bool hasNext() override {
    	return _begin != _end;
    }
};

}
}
