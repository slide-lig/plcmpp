
#include <internals/transactions/ByteIndexedTransactionsList.hpp>
#include <internals/Counters.hpp>

namespace internals {
namespace transactions {


ByteIndexedTransactionsList::ByteIndexedTransactionsList(
		Counters* c) : ByteIndexedTransactionsList(
				c->distinctTransactionLengthSum,
				c->distinctTransactionsCount)
{
}

ByteIndexedTransactionsList::ByteIndexedTransactionsList(
		int32_t transactionsLength, int32_t nbTransactions) :
		IndexedTransactionsList(nbTransactions)
{
	concatenated = new vector<uint8_t>(transactionsLength);
}

unique_ptr<ReusableTransactionIterator> ByteIndexedTransactionsList::getIterator() {
}

unique_ptr<TransactionsList> ByteIndexedTransactionsList::clone() {
}

bool ByteIndexedTransactionsList::compatible(
		Counters* c) {
}

int32_t ByteIndexedTransactionsList::getMaxTransId(
		Counters* c) {
}

void ByteIndexedTransactionsList::writeItem(
		int32_t item) {
}

bool ByteTransIter::isNextPosValid() {
}

void ByteTransIter::removePosVal() {
}

int32_t ByteTransIter::getPosVal() {
}

}
}
