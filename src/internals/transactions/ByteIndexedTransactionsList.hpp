#pragma once

#include <internals/transactions/IndexedTransactionsList.hpp>

namespace internals {

class Counters;

namespace transactions {

class ByteIndexedTransactionsList: public IndexedTransactionsList {

private:
	vector<uint8_t>* concatenated;

public:
	ByteIndexedTransactionsList(Counters* c);
	ByteIndexedTransactionsList(int32_t transactionsLength,
			int32_t nbTransactions);

	unique_ptr<ReusableTransactionIterator> getIterator() override;
	unique_ptr<TransactionsList> clone() override;

	static bool compatible(Counters* c);
	static int32_t getMaxTransId(Counters* c);

protected:
	void writeItem(int32_t item) override;
};

class ByteTransIter: public BasicTransIter {
protected:
	bool isNextPosValid() override;
	void removePosVal() override;
	int32_t getPosVal() override;
};

}
}

