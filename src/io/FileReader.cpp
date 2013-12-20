
#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;

#include <io/FileReader.hpp>
#include <util/Helpers.h>
using namespace util;

namespace io {
const int32_t FileReader::Storage::STARTUP_STORAGE_SIZE = 64*1024;

int FileReader::CopyReader::getTransactionSupport() {
	return 1;
}

int FileReader::CopyReader::next() {
	return (*(current_trans_current_item++));
}

bool FileReader::CopyReader::hasNext() {
	return current_trans_current_item != current_trans_end_items;
}

FileReader::CopyReader::CopyReader(
		Storage* storage, vector<int32_t> *renaming) {
	_storage = storage;
	_renaming = renaming;
	next_trans_start = _storage->begin();
	// used later to show that we have not started reading
}

bool FileReader::CopyReader::hasMoreTransactions() {
	return (next_trans_start != _storage->end());
}

void FileReader::CopyReader::prepareForNextTransaction() {
	vector<int32_t>::iterator it_renaming_src, it_renaming_dst;

	current_trans_start = next_trans_start;
	next_trans_start += (*current_trans_start) +1;
	current_trans_current_item = current_trans_start +1;

	it_renaming_dst = current_trans_current_item;
	for (	it_renaming_src = current_trans_current_item;
			it_renaming_src != next_trans_start;
			it_renaming_src++) {
		int32_t renamed = (*_renaming)[*it_renaming_src];
		if (renamed >= 0) {
			*(it_renaming_dst++) = renamed;
		}
	}

	current_trans_end_items = it_renaming_dst;
	std::sort(current_trans_current_item, current_trans_end_items);
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

	_storage->addNewItem(nextInt);
	return nextInt;
}

bool FileReader::LineReader::hasNext() {
	bool end_of_transaction = (nextChar == '\n');
	if (end_of_transaction)
		_storage->endTransaction();
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
	_storage->startNewTransaction();
}

void FileReader::close(vector<int32_t>* renamingMap) {
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

FileReader::Storage::Storage() : vector<int32_t>() {
	reserve(STARTUP_STORAGE_SIZE);
	current_trans_len = 0;
}

FileReader::Storage::~Storage() {
}

void FileReader::Storage::startNewTransaction() {
	// first integer is the size of the transaction
	// we reserve it
	push_back(0);
	current_trans_len = 0;
}

void FileReader::Storage::addNewItem(int32_t item) {
	push_back(item);
	current_trans_len++;
}

void FileReader::Storage::endTransaction() {
	/* we must update the transaction size
	 * we do not maintain an iterator pointing to the start of
	 * the transaction (i.e. the place where we will store
	 * the size) because, while inserting the items,
	 * if the vector storage is internally resized,
	 * such an iterator would be invalidated.
	 * instead we recompute this position here.
	 */
	auto new_trans_start = end() - current_trans_len -1;
	*new_trans_start = current_trans_len;
}

}
