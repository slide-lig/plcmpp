#pragma once

#include <internals/transactions/TransactionsList.hpp>
#include <internals/TransactionReader.hpp>
#include <util/Iterator.hpp>
#include <util/shortcuts.h>
#include <util/BlocksStorage.hpp>

using util::Iterator;
using util::BlocksStorage;
using util::p_array_int32;
using internals::TransactionReader;
using internals::transactions::CopyableTransactionsList;
using internals::transactions::TransactionsWriter;


namespace io {

/**
 * Reads transactions from an ASCII text file (ALL must be \n-terminated)
 * Each line is a transaction, containing space-separated item IDs as integers
 * (it does not read custom transaction IDs or weights)
 *
 * It directly implements the transactions iterator and copies transactions to memory.
 *
 * Once loaded, call close() and it will be available for another iteration over copied
 * transactions. This second iteration may be done with a rebasing map.
 */

class FileReader
{
	/**
	 * We avoid small allocations by using megabyte pages. Transactions are stored in pages
	 * as in ConcatenatedTransactionsList, although latest indexes may not be used.
	 */

private:

	class CopyReader;
	typedef BlocksStorage<int32_t> Storage;

	class ChainedTransactionReader : public internals::TransactionReader {

		public:
			virtual bool hasMoreTransactions() = 0;
			virtual void prepareForNextTransaction() = 0;
	};


	class CopyReader : public CopyableTransactionsList {

		private:
			Storage::iterator _transactions_iterator;

		/**
		 * read currentPage[currentPageIndex, to[
		 */
		public:
			CopyReader(Storage *storage);

			void copyTo(TransactionsWriter *writer, TidList *tidList,
			    			int32_t *renaming, int32_t coreItem);
	};

	class LineReader : public ChainedTransactionReader {

		private:
			std::ifstream *_file;
			Storage *_storage;

		/**
		 * read currentPage[currentPageIndex, to[
		 */
		public:
			LineReader(Storage *storage, string &path);
			~LineReader();

			int getTransactionSupport() override;
			void getTransactionBounds(int32_t* &begin, int32_t* &end) override;
			bool hasMoreTransactions() override;
			void prepareForNextTransaction() override;
	};

	Storage *_storage;
    ChainedTransactionReader* _reader;

	public:
	    FileReader(string& path);
	    ~FileReader();

	    unique_ptr<FileReader::CopyReader> getSavedTransactions();

		bool hasNext();
		internals::TransactionReader* next();
};

}

