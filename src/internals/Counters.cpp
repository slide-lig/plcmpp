#include <unordered_map>
#include <queue>
#include <memory>
#include <algorithm>
#include <iostream>
using namespace std;

#include <internals/Counters.hpp>
#include <internals/transactions/TransactionsList.hpp>
#include <internals/Dataset.hpp>
#include <util/ItemAndSupport.hpp>
#include <util/ItemsetsFactory.hpp>
#include <io/FileReader.hpp>
using namespace util;

namespace internals {

Counters::Counters(int32_t minimumSupport,
		FileReader* file_reader) {
	minSupport = minimumSupport;

	unordered_map<int32_t, int32_t> supportsMap;
	int biggestItemID = 0;
	FileReader::TransactionReader *transaction;

	// item support and transactions counting

	transactionsCount = 0;
	int32_t item = 0, support;
	int32_t *begin;
	int32_t *end;
	int32_t *it;
	while (file_reader->hasNext()) {
		transaction = file_reader->next();
		++transactionsCount;

		transaction->getTransactionBounds(begin, end);
		for(it = begin; it < end; it++)
		{
			item = *it;
			// the following works because the initial value
			// will be 0 if item was not yet present.
			++(supportsMap[item]);

			// transactions are not sorted yet,
			// so we really have to do that for all items...
			biggestItemID = max(biggestItemID, item);
		}
	}

	distinctTransactionsCount = transactionsCount;
	renaming = make_p_array_int32(biggestItemID + 1, -1);

	// item filtering and final computations : some are infrequent, some
	// belong to closure
	priority_queue<ItemAndSupport> renamingHeap;
	p_vec_int32 new_closure = new vec_int32();

	for (auto iterator = supportsMap.begin(); iterator != supportsMap.end();
			iterator++) {
		item = iterator->first;
		support = iterator->second;

		if (support == transactionsCount) {
			new_closure->push_back(item);
		} else if (support >= minimumSupport) {
			renamingHeap.emplace(item, support);
		} // otherwise item is infrequent : its renaming is already -1, ciao
	}

	closure.reset(new_closure);
	nbFrequents = renamingHeap.size();
	maxFrequent = nbFrequents - 1;
	maxCandidate = maxFrequent + 1;

	supportCounts = make_p_array_int32_no_init(nbFrequents);
	distinctTransactionsCounts = make_p_array_int32_no_init(nbFrequents);
	reverseRenaming = make_p_array_int32_no_init(nbFrequents);
	int remainingSupportsSum = 0;
	auto opt_renaming = renaming->array;
	auto opt_reverseRenaming = reverseRenaming->array;
	auto opt_supportCounts = supportCounts->array;
	auto opt_distinctTransactionsCounts = distinctTransactionsCounts->array;

	int newItemID = 0;

	while (!renamingHeap.empty()) {
		const ItemAndSupport &entry = renamingHeap.top();
		item = entry.item();
		support = entry.support();

		renamingHeap.pop();

		opt_renaming[item] = newItemID;
		opt_reverseRenaming[newItemID] = item;

		opt_supportCounts[newItemID] = support;
		opt_distinctTransactionsCounts[newItemID] = support;

		remainingSupportsSum += support;

		newItemID++;
	}

	compactedArrays = true;
	distinctTransactionLengthSum = remainingSupportsSum;
}

Counters::Counters(const Counters& other) {
	minSupport = other.minSupport;
	transactionsCount = other.transactionsCount;
	distinctTransactionsCount = other.distinctTransactionsCount;
	distinctTransactionLengthSum = other.distinctTransactionLengthSum;
	supportCounts = other.supportCounts;
	distinctTransactionsCounts = other.distinctTransactionsCounts;
	closure = other.closure;
	nbFrequents = other.nbFrequents;
	maxFrequent = other.maxFrequent;
	if (other.reverseRenaming)
		reverseRenaming = other.reverseRenaming;
	else
		reverseRenaming = nullptr;
	if (other.renaming)
		renaming = other.renaming;
	else
		renaming = nullptr;
	compactedArrays = other.compactedArrays;
	maxCandidate = other.maxCandidate;
}

unique_ptr<Counters> Counters::clone() {
	// create a clone using the copy constructor
	Counters *cloned = new Counters(*this);
	return unique_ptr<Counters>(cloned);
}

int32_t Counters::getMaxFrequent() {
	return maxFrequent;
}

shp_array_int32 Counters::getRenaming() {
	return renaming;
}

shp_array_int32 Counters::getReverseRenaming() {
	return reverseRenaming;
}

shp_vec_int32 Counters::getClosure() {
	return closure;
}

void Counters::reuseRenaming(shp_array_int32 olderReverseRenaming) {
	reverseRenaming = olderReverseRenaming;
}

shp_array_int32 Counters::compressRenaming(
		shp_array_int32 olderReverseRenaming) {

	auto new_renaming_size = supportCounts->size();
	auto new_renaming = make_p_array_int32_no_init(new_renaming_size);
	auto opt_new_renaming = new_renaming->array;

	if (new_renaming_size <= UINT8_MAX)
	{	// we consider there will be no added value to compress,
		// we just mark the unfrequent items with the value -1.
		int32_t item = 0;
		auto end_supportCounts = supportCounts->end();
		for (auto curr_supportCounts = supportCounts->begin();
				curr_supportCounts < end_supportCounts;
				++curr_supportCounts) {
			if (*curr_supportCounts > 0) {
				opt_new_renaming[item] = item;
			}
			else {
				opt_new_renaming[item] = -1;
			}
			++item;
		}
		reverseRenaming = olderReverseRenaming;

		maxFrequent = item - 1;
		compactedArrays = false;
	}
	else
	{
		reverseRenaming = make_p_array_int32_no_init(nbFrequents);
		auto opt_supportCounts = supportCounts->array;
		auto opt_reverseRenaming = reverseRenaming->array;
		auto opt_olderReverseRenaming = olderReverseRenaming->array;
		auto opt_distinctTransactionsCounts = distinctTransactionsCounts->array;

		// we will always have newItemID <= item
		int newItemID = 0;
		int greatestBelowMaxCandidate = -2;

		int32_t item = 0;
		auto end_supportCounts = supportCounts->end();
		for (auto curr_supportCounts = supportCounts->begin();
				curr_supportCounts < end_supportCounts;
				++curr_supportCounts) {
			if (*curr_supportCounts > 0) {
				opt_new_renaming[item] = newItemID;
				opt_reverseRenaming[newItemID] = opt_olderReverseRenaming[item];

				opt_distinctTransactionsCounts[newItemID] = opt_distinctTransactionsCounts[item];
				opt_supportCounts[newItemID] = *curr_supportCounts;

				if (item < maxCandidate) {
					greatestBelowMaxCandidate = newItemID;
				}

				newItemID++;
			} else {
				opt_new_renaming[item] = -1;
			}
			++item;
		}

		maxCandidate = greatestBelowMaxCandidate + 1;
		maxFrequent = newItemID - 1;
		compactedArrays = true;

	}

	renaming = new_renaming;

	return new_renaming;
}

int32_t Counters::getMaxCandidate() {
	return maxCandidate;
}

unique_ptr<FrequentsIterator> Counters::getExtensionsIterator() {
	return unique_ptr<FrequentsIterator>(
			new ExtensionsIterator(this, maxCandidate));
}

ExtensionsIterator::ExtensionsIterator(Counters* Counters, int32_t to) {
	_counters = Counters;
	index = 0;
	max = to;
}

int32_t ExtensionsIterator::next() {
	if (_counters->compactedArrays) {
		int32_t nextIndex = index++;
		if (nextIndex < max) {
			return nextIndex;
		} else {
			return -1;
		}
	} else {
		auto opt_supportCounts = _counters->supportCounts->array;
		while (true) {
			int32_t nextIndex = index++;
			if (nextIndex < max) {
				if (opt_supportCounts[nextIndex] > 0) {
					return nextIndex;
				}
			} else {
				return -1;
			}
		}
	}
	return 0;	// never reached but avoids a warning
}

int32_t ExtensionsIterator::peek() {
	return index;
}

int32_t ExtensionsIterator::last() {
	return max;
}

}
