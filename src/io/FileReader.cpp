
#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;

#include <io/FileReader.hpp>
#include <util/Helpers.h>
using namespace util;

namespace io {

int FileReader::CopyReader::getTransactionSupport() {
	return 1;
}

int FileReader::CopyReader::next() {
	return (*(current_trans_start++));
}

bool FileReader::CopyReader::hasNext() {
	return current_trans_start != current_trans_end;
}

FileReader::CopyReader::CopyReader(
		Storage* storage, p_array_int32 renaming) {
	_transactions_iterator = storage->getIterator();
	_renaming = renaming;
	current_trans_start = nullptr;
	current_trans_end = nullptr;
}

bool FileReader::CopyReader::hasMoreTransactions() {
	return _transactions_iterator.has_more_blocks();
}

void FileReader::CopyReader::prepareForNextTransaction() {
	int32_t* it_renaming_src;
	int32_t* it_renaming_dst;

	_transactions_iterator.next_block(
			current_trans_start, current_trans_end);

	it_renaming_dst = current_trans_start;
	for (	it_renaming_src = current_trans_start;
			it_renaming_src != current_trans_end;
			it_renaming_src++) {
		int32_t renamed = (*_renaming)[*it_renaming_src];
		if (renamed >= 0) {
			*(it_renaming_dst++) = renamed;
		}
	}

	current_trans_end = it_renaming_dst;
	std::sort(current_trans_start, current_trans_end);
}

FileReader::LineReader::LineReader(Storage *storage, string &path) {
	_storage = storage;
	_file = new std::ifstream(path);

	if (!_file->is_open())
	{
		cerr << "Could not open " << path << ". Aborting." << endl;
		abort();
	}

	nextChar = _file->get();
}

FileReader::LineReader::~LineReader()
{
	_file->close();
	delete _file;
}

int FileReader::LineReader::getTransactionSupport() {
	return 1;
}

int FileReader::LineReader::next() {
	int nextInt = -1;
	while (nextChar == ' ')
		nextChar = _file->get();

	while('0' <= nextChar && nextChar <= '9') {
		if (nextInt < 0) {
			nextInt = nextChar - '0';
		} else {
			nextInt = (10*nextInt) + (nextChar - '0');
		}
		nextChar = _file->get();
	}

	while (nextChar == ' ')
		nextChar = _file->get();

	_storage->push_back(nextInt);
	return nextInt;
}

bool FileReader::LineReader::hasNext() {
	bool end_of_transaction = (nextChar == '\n');
	if (end_of_transaction)
		_storage->end_block();
	return !end_of_transaction;
}

bool FileReader::LineReader::hasMoreTransactions() {
	// skip empty lines
	while ((nextChar == '\n') &&
			(_file->good())) {
		nextChar = _file->get();
	}
	return _file->good();
}

void FileReader::LineReader::prepareForNextTransaction() {
	// skip empty lines
	while (nextChar == '\n') {
		nextChar = _file->get();
	}
	_storage->start_block();
}

void FileReader::close(p_array_int32 renamingMap) {
	delete _reader;
	_reader = new FileReader::CopyReader(_storage, renamingMap);
}

bool FileReader::hasNext() {
	return _reader->hasMoreTransactions();
}

internals::TransactionReader* FileReader::next() {
	_reader->prepareForNextTransaction();
	return _reader;
}

void FileReader::remove() {
	cerr << "Unsupported! Aborting." << endl;
	abort();
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
