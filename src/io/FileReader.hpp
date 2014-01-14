#pragma once

#include <internals/TransactionReader.hpp>
#include <util/Iterator.hpp>
#include <util/shortcuts.h>


using util::Iterator;
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

	class Storage : public vector<int32_t> {
	private:
		static const int32_t STARTUP_STORAGE_SIZE;

		/* transactions are coded the following way:
		 * - 1st integer is its size
		 * - next ones are the items
	     */
	    vector<int32_t>::iterator current_trans_next_item;
	    int32_t current_trans_len;

	public:
	    Storage();
	    ~Storage();

	    void startNewTransaction();
	    void addNewItem(int32_t item);
	    void endTransaction();
	};

	class ChainedTransactionReader : public internals::TransactionReader {

		public:
			virtual bool hasMoreTransactions() = 0;
			virtual void prepareForNextTransaction() = 0;
	};


	class CopyReader : public ChainedTransactionReader {

		private:
			vector<int32_t>::iterator current_trans_start;
			vector<int32_t>::iterator next_trans_start;
			vector<int32_t>::iterator current_trans_current_item;
			vector<int32_t>::iterator current_trans_end_items;
			Storage *_storage;
			p_array_int32 _renaming;

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

