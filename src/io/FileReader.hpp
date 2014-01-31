#pragma once

#include <internals/TransactionReader.hpp>
#include <util/Iterator.hpp>
#include <util/shortcuts.h>
#include <util/BlocksStorage.hpp>

using util::Iterator;
using util::BlocksStorage;
using util::p_array_int32;
using internals::TransactionReader;

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

class FileReader : public Iterator<TransactionReader*>
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


	class CopyReader : public ChainedTransactionReader {

		private:
			Storage::iterator _transactions_iterator;
			p_array_int32 _renaming;
			int32_t* current_trans_start;
			int32_t* current_trans_end;

		/**
		 * read currentPage[currentPageIndex, to[
		 */
		public:
			CopyReader(Storage *storage, p_array_int32 renaming);

			int getTransactionSupport() override;
			int next() override;
			bool hasNext() override;
			bool hasMoreTransactions() override;
			void prepareForNextTransaction() override;
	};

	class LineReader : public ChainedTransactionReader {

		private:
			std::ifstream *_file;
			char nextChar;
			Storage *_storage;

		/**
		 * read currentPage[currentPageIndex, to[
		 */
		public:
			LineReader(Storage *storage, string &path);
			~LineReader();

			int getTransactionSupport() override;
			int next() override;
			bool hasNext() override;
			bool hasMoreTransactions() override;
			void prepareForNextTransaction() override;
	};

	Storage *_storage;
    ChainedTransactionReader* _reader;

	public:
	    FileReader(string& path);
	    ~FileReader();

		void close(p_array_int32 renamingMap);

		bool hasNext();
		internals::TransactionReader* next();
		void remove();
};

}

