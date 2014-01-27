
#pragma once

#include <internals/transactions/TransactionsList.hpp>
#include <internals/transactions/ReusableTransactionIterator.hpp>
#include <internals/transactions/TransactionsWriter.hpp>
#include <util/Iterator.hpp>
#include <util/shortcuts.h>

using namespace util;

namespace internals {
namespace transactions {

class IndexedReusableIterator;

class IndexedTransactionsList: public TransactionsList {

private:
	p_array_int32 _indexAndFreqs;
	int32_t* _indexAndFreqs_fast;
	uint32_t _indexAndFreqs_size;
	int32_t _size;

protected:
	int32_t writeIndex;
	IndexedTransactionsList(const IndexedTransactionsList& other);

public:
	IndexedTransactionsList(int32_t nbTransactions);
	~IndexedTransactionsList();

	void positionIterator(int32_t transaction,
			IndexedReusableIterator *iter);
	int32_t getTransSupport(int32_t trans);
	void setTransSupport(int32_t trans, int32_t s);
	void beginTransaction(int32_t transId, int32_t support);
	virtual void writeItem(int32_t item) = 0;
	int32_t findNext(int32_t nextPos);

	unique_ptr<Iterator<int32_t>> getIdIterator() override;
	unique_ptr<TransactionsWriter> getWriter() override;
	int32_t size() override;
};

class IndexedReusableIterator: public ReusableTransactionIterator {
private:
	int transNum;
	IndexedTransactionsList *_tlist;

public:
	IndexedReusableIterator(IndexedTransactionsList *tlist);
	virtual void set(int32_t begin, int32_t end) = 0;
	void setTransaction(int32_t transaction);
	int32_t getTransactionSupport();
	void setTransactionSupport(int32_t s);
};

class BasicTransIter : public IndexedReusableIterator
{
protected:
    int32_t _pos;
    int32_t _nextPos;

private:
    int32_t _end;
    bool _first;

protected:
    void set(int32_t begin, int32_t end) override;

private:
    void findNext();

protected:
    virtual bool isNextPosValid() = 0;
    virtual void removePosVal() = 0;
    virtual int32_t getPosVal() = 0;

public:
    BasicTransIter(IndexedTransactionsList *tlist);
    int32_t next() override;
    bool hasNext() override;
    void remove() override;
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
