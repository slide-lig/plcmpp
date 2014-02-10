
#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;

#include <io/FileReader.hpp>
#include <util/Helpers.h>
using namespace util;

namespace io {

FileReader::CopyReader::CopyReader(
		Storage* storage) {
	_transactions_iterator = storage->getIterator();
}

void FileReader::CopyReader::copyTo(TransactionsWriter* writer,
		TidList* tidList, int32_t* renaming, int32_t coreItem) {

	int32_t *begin;
	int32_t *end;
	int32_t *it;
	int32_t item;
	int32_t transId;
	int32_t* it_renaming_src;
	int32_t* it_renaming_dst;

	while (_transactions_iterator.has_more_blocks()) {

		// retrieve transaction bounds
		_transactions_iterator.next_block(
				begin, end);

		// sort the transaction, and
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
		std::sort(begin, end);

		transId = writer->beginTransaction(1);

		for(it = begin; it < end; ++it)
		{
			item = *it;
			writer->addItem(item);
			tidList->addTransaction(item, transId);
		}

		writer->endTransaction(coreItem);
	}

}

FileReader::LineReader::LineReader(Storage *storage, string &path) {
	_storage = storage;
	_file = new std::ifstream(path);

	if (!_file->is_open())
	{
		cerr << "Could not open " << path << ". Aborting." << endl;
		abort();
	}
}

FileReader::LineReader::~LineReader()
{
	_file->close();
	delete _file;
}

int FileReader::LineReader::getTransactionSupport() {
	return 1;
}

enum READ_STATE {
	SPACES,
	INTEGER
};
void FileReader::LineReader::getTransactionBounds(int32_t* &begin, int32_t* &end) {
	string line;
	getline(*_file, line);
	enum READ_STATE state = SPACES;
	uint cur_int = 0;
	for (char c : line)
	{
		if (c == ' ')
		{
			if (state == SPACES)
				continue;
			else
			{
				_storage->push_back(cur_int);
				cur_int = 0;
				state = SPACES;
			}
		}
		else // a digit 0-9
		{
			if (state == SPACES)
				state = INTEGER;

			cur_int = (10*cur_int) + (c - '0');
		}
	}

	// do not forget the last one...
	if (state == INTEGER)
	{
		_storage->push_back(cur_int);
	}

	_storage->end_block(begin, end);
}

bool FileReader::LineReader::hasMoreTransactions() {
	return _file->good();
}

void FileReader::LineReader::prepareForNextTransaction() {
	_storage->start_block();
}

unique_ptr<FileReader::CopyReader> FileReader::getSavedTransactions() {
	return unique_ptr<FileReader::CopyReader>(
			new FileReader::CopyReader(_storage));
}

bool FileReader::hasNext() {
	return _reader->hasMoreTransactions();
}

internals::TransactionReader* FileReader::next() {
	_reader->prepareForNextTransaction();
	return _reader;
}

FileReader::FileReader(string& path) {
	_storage = new FileReader::Storage();
	_reader = new FileReader::LineReader(_storage, path);
}

FileReader::~FileReader() {
	delete _reader;
	delete _storage;
}

}
