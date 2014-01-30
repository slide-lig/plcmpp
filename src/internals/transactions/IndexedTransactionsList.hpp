
#pragma once

#include <internals/transactions/TransactionsList.hpp>
#include <internals/transactions/ReusableTransactionIterator.hpp>
#include <internals/transactions/TransactionsWriter.hpp>
#include <util/Iterator.hpp>
#include <util/shortcuts.h>

using namespace util;

namespace internals {
namespace transactions {

class IndexedTransactionsList: public TransactionsList {

protected:
	p_array_int32 _indexAndFreqs;
	int32_t* _indexAndFreqs_fast;
	int32_t _indexAndFreqs_used_size;
	int32_t _size;

protected:
	int32_t writeIndex;
	IndexedTransactionsList(const IndexedTransactionsList& other);

public:
	IndexedTransactionsList(int32_t nbTransactions);
	~IndexedTransactionsList();

	int32_t getTransSupport(int32_t trans);
	void setTransSupport(int32_t trans, int32_t s);
	void beginTransaction(int32_t transId, int32_t support);
	virtual void writeItem(int32_t item) = 0;
	int32_t findNext(int32_t nextPos);

	unique_ptr<Iterator<int32_t>> getIdIterator() override;
	unique_ptr<TransactionsWriter> getWriter() override;
	int32_t size() override;
};

class Writer : public TransactionsWriter
{
private:
    int32_t transId;
    IndexedTransactionsList *_tlist;

public:
    Writer(IndexedTransactionsList *tlist);
    int32_t beginTransaction(int32_t support) override;
    void addItem(int32_t item) override;
    void endTransaction() override;
};

class IdIter : public Iterator<int32_t>
{
private:
    int32_t pos;
    int32_t nextPos;
    IndexedTransactionsList *_tlist;

public:
    IdIter(IndexedTransactionsList *tlist);
    int32_t next() override;
    bool hasNext() override;

private:
    void findNext();
};

}
}
