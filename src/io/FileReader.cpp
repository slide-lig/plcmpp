
#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;

#include <io/FileReader.hpp>

const int32_t io::FileReader::Storage::STARTUP_STORAGE_SIZE = 64*1024;

int io::FileReader::CopyReader::getTransactionSupport() {
	return 1;
}

int io::FileReader::CopyReader::next() {
	return (*(current_trans_current_item++));
}

bool io::FileReader::CopyReader::hasNext() {
	return current_trans_current_item != current_trans_end_items;
}

io::FileReader::CopyReader::CopyReader(
		Storage* storage, vector<int32_t> *renaming) {
	_storage = storage;
	_renaming = renaming;
	next_trans_start = _storage->begin();
	// used later to show that we have not started reading
}

bool io::FileReader::CopyReader::hasMoreTransactions() {
	return (current_trans_end_items == _storage->end());
}

void io::FileReader::CopyReader::prepareForNextTransaction() {
	vector<int32_t>::iterator it_renaming_src, it_renaming_dst;

	current_trans_start = next_trans_start;
	next_trans_start += (*current_trans_start) +1;
	current_trans_current_item = current_trans_start +1;

	it_renaming_dst = current_trans_current_item;
	for (	it_renaming_src = current_trans_current_item;
			it_renaming_src != next_trans_start;
			it_renaming_src++) {
		uint32_t renamed = (*_renaming)[*it_renaming_src];
		if (renamed >= 0) {
			*(it_renaming_dst++) = renamed;
		}
	}

	current_trans_end_items = it_renaming_dst;
	std::sort(current_trans_current_item, current_trans_end_items);
}

io::FileReader::LineReader::LineReader(Storage *storage, string &path) {
	_storage = storage;
	_file = new std::ifstream(path);

	if (!_file->is_open())
	{
		cerr << "Could not open " << path << ". Aborting." << endl;
		abort();
	}

	nextChar = _file->get();
}

io::FileReader::LineReader::~LineReader()
{
	_file->close();
	delete _file;
}

int io::FileReader::LineReader::getTransactionSupport() {
	return 1;
}

int io::FileReader::LineReader::next() {
	nextChar = _file->get();
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

bool io::FileReader::LineReader::hasNext() {
	bool end_of_transaction = (nextChar != '\n');
	if (end_of_transaction)
		_storage->endTransaction();
	return end_of_transaction;
}

bool io::FileReader::LineReader::hasMoreTransactions() {
	// skip empty lines
	while ((nextChar == '\n') &&
			(_file->good())) {
		nextChar = _file->get();
	}
	return _file->good();
}

void io::FileReader::LineReader::prepareForNextTransaction() {
	// skip empty lines
	while (nextChar == '\n') {
		nextChar = _file->get();
	}
	_storage->startNewTransaction();
}

void io::FileReader::close(vector<int32_t>* renamingMap) {
	delete _reader;
	_reader = new io::FileReader::CopyReader(_storage, renamingMap);
}

bool io::FileReader::hasNext() {
	return _reader->hasMoreTransactions();
}

internals::TransactionReader* io::FileReader::next() {
	_reader->prepareForNextTransaction();
	return _reader;
}

void io::FileReader::remove() {
	cerr << "Unsupported! Aborting." << endl;
	abort();
}

io::FileReader::FileReader(string& path) {
	_storage = new io::FileReader::Storage();
	_reader = new io::FileReader::LineReader(_storage, path);
}

io::FileReader::~FileReader() {
	delete _reader;
	delete _storage;
}

io::FileReader::Storage::Storage() : vector<int32_t>(STARTUP_STORAGE_SIZE) {
	current_trans_next_item = begin();
	current_trans_len = 0;
}

io::FileReader::Storage::~Storage() {
}

void io::FileReader::Storage::startNewTransaction() {
	vector<int32_t>::iterator new_trans_start =
			current_trans_next_item;
	if (current_trans_next_item == begin())
	{
		new_trans_start = begin();
	}
	else
	{
		new_trans_start = current_trans_next_item;
	}
	// first integer is the size of the transaction
	// we must shift to next one
	current_trans_next_item = new_trans_start+1;
	current_trans_len = 0;
}

void io::FileReader::Storage::addNewItem(int32_t item) {
	*(current_trans_next_item++) = item;
	current_trans_len++;
}

void io::FileReader::Storage::endTransaction() {
	/* we must update the transaction size
	 * we do not maintain an iterator pointing to the start of
	 * the transaction (i.e. the place where we will store
	 * the size) because, while inserting the items,
	 * if the vector storage is internally resized,
	 * such an iterator would be invalidated.
	 * instead we recompute this position here.
	 */
	vector<int32_t>::iterator new_trans_start =
			current_trans_next_item - current_trans_len -1;
	*new_trans_start = current_trans_len;
}
