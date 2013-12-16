

#pragma once

#include <internals/TransactionReader.hpp>
#include <util/Iterator.hpp>

using namespace util;

namespace internals {

class TransactionsIteratorDecorator : public Iterator<TransactionReader*>
{
protected:
	class FilteredTransaction : public virtual TransactionReader
	{
	protected:
	    TransactionReader* _wrapped;
	    int32_t _next;
	    bool _hasNext;
	    TransactionsIteratorDecorator *_deco;

	public:
	    void reset(TransactionReader* filtered);

	protected:
	    void findNext();

	public:
	    int32_t getTransactionSupport() override;
	    int32_t next() override;
	    bool hasNext() override;

	    FilteredTransaction(
	    		TransactionsIteratorDecorator *deco,
	    		TransactionReader* filtered);
	};

protected:
	Iterator<TransactionReader*>* wrapped;
    FilteredTransaction* instance;

public:
    bool hasNext() override;
    TransactionReader* next() override;
    virtual int32_t transform(int32_t in) = 0;

    TransactionsIteratorDecorator(
    		Iterator<TransactionReader*>* filtered);
};
}
