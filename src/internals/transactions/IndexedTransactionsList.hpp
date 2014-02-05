
#pragma once

#include <internals/transactions/TransactionsList.hpp>
#include <internals/transactions/ReusableTransactionIterator.hpp>
#include <internals/transactions/TransactionsWriter.hpp>
#include <util/Iterator.hpp>
#include <util/shortcuts.h>

using namespace util;

namespace internals {
namespace transactions {

template <class T, class IteratorT>
class Template_IndexedReusableIterator;

template <class T>
class IndexedTransactionsList: public TransactionsList {

protected:
	typedef Template_ReusableTransactionIterator<T> NativeIterator;
	T* _concatenated;

	typedef struct {
		T* start_transaction;
		T* end_transaction;
		T* end_prefix;
		int32_t support;
	} descTransaction;

	descTransaction* _transactions_info;
	int32_t _num_allocated_transactions;
	int32_t _num_real_transactions;
	T* _writeIndex;

public:
	static const T MAX_VALUE;

	IndexedTransactionsList(Counters* c);
	IndexedTransactionsList(int32_t transactionsLength,
			int32_t nbTransactions);
	~IndexedTransactionsList();

	int32_t getTransSupport(int32_t trans);
	void setTransSupport(int32_t trans, int32_t s);
	void beginTransaction(int32_t transId, int32_t support);
	void endTransaction(int32_t transId, int32_t core_item);
	void writeItem(int32_t item);
	int32_t findNext(int32_t nextPos);

	unique_ptr<Iterator<int32_t>> getIdIterator() override;
	unique_ptr<TransactionsWriter> getWriter() override;
	int32_t size() override;
	unique_ptr<ReusableTransactionIterator> getIterator() override;
    void compress() override;

	static bool compatible(Counters* c);
	static int32_t getMaxTransId(Counters* c);

	template <class IteratorT>
	void positionIterator(int32_t transaction,
			Template_IndexedReusableIterator<T, IteratorT> *iter);

private:
	template <class IteratorT>
	unique_ptr<Template_ReusableTransactionIterator<IteratorT> > getIteratorWithType();

    static void sort(
    		descTransaction* transactions_info,
    		int32_t* start, int32_t* end);
    static int32_t merge(
    		descTransaction* transactions_info,
    		int32_t t1,
    		int32_t t2);

};

template <class T>
class Writer : public TransactionsWriter
{
private:
    int32_t transId;
    IndexedTransactionsList<T> *_tlist;

public:
    Writer(IndexedTransactionsList<T> *tlist);
    int32_t beginTransaction(int32_t support) override;
    void addItem(int32_t item) override;
    void endTransaction(int32_t core_item) override;
};

template <class T>
class IdIter : public Iterator<int32_t>
{
private:
    int32_t pos;
    int32_t nextPos;
    IndexedTransactionsList<T> *_tlist;

public:
    IdIter(IndexedTransactionsList<T> *tlist);
    int32_t next() override;
    bool hasNext() override;

private:
    void findNext();
};

}
}

/* Template classes, we need their definition */
#include <internals/transactions/IndexedTransactionsList_impl_main.hpp>
#include <internals/transactions/IndexedTransactionsList_impl_compress.hpp>
