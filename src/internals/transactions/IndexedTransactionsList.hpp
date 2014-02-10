
#pragma once

#include <internals/transactions/TransactionsList.hpp>
#include <internals/transactions/TransactionsWriter.hpp>
#include <util/Iterator.hpp>
#include <util/shortcuts.h>
#include <util/SimpleDigest.h>

using namespace util;

namespace internals {
namespace transactions {

template <class itemT, class IteratorT>
class Template_IndexedReusableIterator;

template <class itemT>
class IndexedTransactionsList: public TransactionsList {

protected:
	itemT* _concatenated;

	typedef struct {
		SimpleDigest::Type prefix_hash;
		int32_t support;
		itemT* start_transaction;
		itemT* end_transaction;
		itemT* end_prefix;
	} descTransaction;

	descTransaction* _transactions_info;
	int32_t _num_allocated_transactions;
	int32_t _num_real_transactions;
	itemT* _writeIndex;

public:
	static const itemT MAX_VALUE;

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
    void compress() override;

	static bool compatible(Counters* c);
	static int32_t getMaxTransId(Counters* c);

	void countSubList(TidList::ItemTidList *tidlist,
	    		int32_t& transactionsCount, int32_t& distinctTransactionsCount,
	    		int32_t* supportCounts, int32_t* distinctTransactionsCounts,
	    		int32_t extension, int32_t maxItem) override;

	void copyTo(TidList::ItemTidList* item_tidList,
	    		TransactionsWriter* writer, TidList* new_tidList,
	    		int32_t* renaming, int32_t coreItem) override;

private:
    static void sort(
    		descTransaction* transactions_info,
    		int32_t* start, int32_t* end);
    static int32_t merge(
    		descTransaction* transactions_info,
    		int32_t t1,
    		int32_t t2);

};

template <class itemT>
class Writer : public TransactionsWriter
{
private:
    int32_t transId;
    IndexedTransactionsList<itemT> *_tlist;

public:
    Writer(IndexedTransactionsList<itemT> *tlist);
    int32_t beginTransaction(int32_t support) override;
    void addItem(int32_t item) override;
    void endTransaction(int32_t core_item) override;
};

template <class itemT>
class IdIter : public Iterator<int32_t>
{
private:
    int32_t pos;
    int32_t nextPos;
    IndexedTransactionsList<itemT> *_tlist;

public:
    IdIter(IndexedTransactionsList<itemT> *tlist);
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
