/*******************************************************************************
 * Copyright (c) 2014 Etienne Dublé, Martin Kirchgessner, Vincent Leroy, Alexandre Termier, CNRS and Université Joseph Fourier.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the LICENSE.txt file joined with this program.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

#pragma once

#include <internals/transactions/TransactionsList.hpp>
#include <util/Iterator.hpp>
#include <util/shortcuts.h>
#include <util/BlocksStorage.hpp>

using util::Iterator;
using util::BlocksStorage;
using util::p_array_int32;


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

public:
	class TransactionReader {

		public:
			virtual ~TransactionReader() {};
			virtual int32_t getTransactionSupport() = 0;
			virtual void getTransactionBounds(int32_t* &begin, int32_t* &end) = 0;
			virtual bool hasMoreTransactions() = 0;
			virtual void prepareForNextTransaction() = 0;
	};

private:
	class CopyReader;
	typedef BlocksStorage<int32_t> Storage;

	class CopyReader {

		private:
			Storage::iterator _transactions_iterator;

		/**
		 * read currentPage[currentPageIndex, to[
		 */
		public:
			CopyReader(Storage *storage);

			template <class TransactionsWriter>
			void copyTo(TransactionsWriter *writer, TidList *tidList,
			    			int32_t *renaming, int32_t max_candidate);
	};

	class LineReader : public TransactionReader {

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
    TransactionReader* _reader;

	public:
	    FileReader(string& path);
	    ~FileReader();

	    unique_ptr<FileReader::CopyReader> getSavedTransactions();

		bool hasNext();
		TransactionReader* next();
};

template <class TransactionsWriter>
void FileReader::CopyReader::copyTo(TransactionsWriter* writer,
		TidList* tidList, int32_t* renaming, int32_t max_candidate) {

	int32_t *begin;
	int32_t *end;
	int32_t *it;
	int32_t item;
	int32_t transId;
	int32_t* it_renaming_src;
	int32_t* it_renaming_dst;
	typename TransactionsWriter::base_type *write_index, *start_index;

	while (_transactions_iterator.has_more_blocks()) {

		// retrieve transaction bounds
		_transactions_iterator.next_block(
				begin, end);

		// rename / filter the items
		it_renaming_dst = begin;
		for (	it_renaming_src = begin;
				it_renaming_src != end;
				it_renaming_src++) {
			int32_t renamed = renaming[*it_renaming_src];
			if (renamed >= 0) {
				*(it_renaming_dst++) = renamed;
			}
		}
		end = it_renaming_dst;
		if (begin == end) continue;

		transId = writer->beginTransaction(1, write_index);
		start_index = write_index;

		for(it = begin; it < end; ++it)
		{
			item = *it;
			*(write_index++) = item;
			tidList->addTransaction(item, transId);
		}

		// sort the transaction
		std::sort(start_index, write_index);

		writer->endTransaction(write_index);
	}
}

}

