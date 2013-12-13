#pragma once

#include <internals/transactions/IndexedTransactionsList.hpp>

namespace internals {

class Counters;

namespace transactions {

template <class T>
class Template_IndexedTransactionsList: public IndexedTransactionsList {

private:
	vector<T>* _concatenated;

public:
	static const T MAX_VALUE;

	Template_IndexedTransactionsList(Counters* c);
	Template_IndexedTransactionsList(int32_t transactionsLength,
			int32_t nbTransactions);
	Template_IndexedTransactionsList(
			const Template_IndexedTransactionsList& other);

	unique_ptr<ReusableTransactionIterator> getIterator() override;
	unique_ptr<TransactionsList> clone() override;

	static bool compatible(Counters* c);
	static int32_t getMaxTransId(Counters* c);

protected:
	void writeItem(int32_t item) override;
};

template <class T>
class Template_TransIter: public BasicTransIter {
private:
	vector<T>* _concatenated;

public:
	Template_TransIter(
			IndexedTransactionsList *tlist,
			vector<T>* concatenated);
protected:
	bool isNextPosValid() override;
	void removePosVal() override;
	int32_t getPosVal() override;
};

}
}

/* Template classes, we need their definition */
#include <internals/transactions/Template_IndexedTransactionsList_impl.hpp>
