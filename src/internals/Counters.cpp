#include <map>
#include <queue>
#include <memory>
#include <algorithm>
#include <iostream>
using namespace std;

#include <internals/Counters.hpp>
#include <internals/TransactionReader.hpp>
#include <util/ItemAndSupport.hpp>
#include <util/ItemsetsFactory.hpp>
using namespace util;

namespace internals {

shp_vec_int32 make_p_vec_int32(uint32_t size, int32_t init_value)
{
	return make_shared<vec_int32>(size, init_value);
}


shp_array_int32 make_p_array_int32(uint32_t size)
{
	return make_shared<array_int32>(size);
}

shp_array_int32 make_p_array_int32(uint32_t size, int32_t init_value)
{
	return make_shared<array_int32>(size, init_value);
}

Counters::Counters(
		int32_t minimumSupport,
		Iterator<TransactionReader*>* transactions,
		int32_t extension,
		int32_t maxItem) {

	renaming = nullptr;
	minSupport = minimumSupport;
	supportCounts = make_p_array_int32(maxItem + 1, 0);
	distinctTransactionsCounts = make_p_array_int32(maxItem + 1, 0);
	reverseRenaming = nullptr;
	auto opt_supportCounts = supportCounts->array;
	auto opt_distinctTransactionsCounts = distinctTransactionsCounts->array;

	// item support and transactions counting

	int weightsSum = 0;

	while (transactions->hasNext()) {
		TransactionReader* transaction = transactions->next();
		int weight = transaction->getTransactionSupport();

		if (weight > 0) {
			if (transaction->hasNext()) {
				weightsSum += weight;
			}

			while (transaction->hasNext()) {
				int item = transaction->next();
				if (item <= maxItem) {
					opt_supportCounts[item] += weight;
					opt_distinctTransactionsCounts[item]++;
				}
			}
		}
	}

	transactionsCount = weightsSum;
	distinctTransactionsCount = transactionsCount;

	// ignored items
	opt_supportCounts[extension] = 0;
	opt_distinctTransactionsCounts[extension] = 0;
	maxCandidate = extension;

	// item filtering and final computations : some are infrequent, some
	// belong to closure

	p_vec_int32 new_closure = new vec_int32();
	uint32_t remainingDistinctTransLengths = 0;
	uint32_t remainingFrequents = 0;

	auto it_supportCounts_end = supportCounts->end();
	auto it_supportCounts = supportCounts->begin();
	auto it_distinctTransactionsCounts =
			distinctTransactionsCounts->begin();
	uint32_t i;
	uint32_t biggestItemID = 0;

	for (i = 0;
		 it_supportCounts != it_supportCounts_end;
		 ++it_supportCounts, ++i, ++it_distinctTransactionsCounts)
	{
		int32_t& ref_supportCount = *it_supportCounts;
		if (ref_supportCount < minimumSupport) {
			ref_supportCount = 0;
			(*it_distinctTransactionsCounts) = 0;
		} else if (ref_supportCount == transactionsCount) {
			new_closure->push_back(i);
			ref_supportCount = 0;
			(*it_distinctTransactionsCounts) = 0;
		} else {
			biggestItemID = i;
			remainingFrequents++;
			remainingDistinctTransLengths += (*it_distinctTransactionsCounts);
		}
	}

	closure.reset(new_closure);
	distinctTransactionLengthSum = remainingDistinctTransLengths;
	nbFrequents = remainingFrequents;
	maxFrequent = biggestItemID;

	compactedArrays = false;
}

Counters::Counters(int32_t minimumSupport,
		Iterator<TransactionReader*>* transactions) {
	minSupport = minimumSupport;

	map<int32_t, int32_t> *supportsMap = new map<int32_t, int32_t>();
	int biggestItemID = 0;

	// item support and transactions counting

	int transactionsCounter = 0;
	while (transactions->hasNext()) {
		TransactionReader *transaction = transactions->next();
		transactionsCounter++;

		while (transaction->hasNext()) {
			int item = transaction->next();
			biggestItemID = max(biggestItemID, item);
			// the following works because the initial value
			// will be 0 if item was not yet present.
			(*supportsMap)[item]++;
		}
	}

	transactionsCount = transactionsCounter;
	distinctTransactionsCount = transactionsCounter;
	renaming = make_p_array_int32(biggestItemID + 1, -1);

	// item filtering and final computations : some are infrequent, some
	// belong to closure
	priority_queue<ItemAndSupport> renamingHeap;
	p_vec_int32 new_closure = new vec_int32();

	for (auto iterator = supportsMap->begin(); iterator != supportsMap->end();
			iterator++) {
		int32_t item = iterator->first;
		int32_t supportCount = iterator->second;

		if (supportCount == transactionsCount) {
			new_closure->push_back(item);
		} else if (supportCount >= minimumSupport) {
			renamingHeap.emplace(item, supportCount);
		} // otherwise item is infrequent : its renaming is already -1, ciao
	}

	delete supportsMap;

	closure.reset(new_closure);
	nbFrequents = renamingHeap.size();
	maxFrequent = nbFrequents - 1;
	maxCandidate = maxFrequent + 1;

	supportCounts = make_p_array_int32(nbFrequents);
	distinctTransactionsCounts = make_p_array_int32(nbFrequents);
	reverseRenaming = make_p_array_int32(nbFrequents);
	int remainingSupportsSum = 0;

	int newItemID = 0;

	while (!renamingHeap.empty()) {
		const ItemAndSupport &entry = renamingHeap.top();
		int32_t item = entry.item();
		int32_t support = entry.support();

		renamingHeap.pop();

		(*renaming)[item] = newItemID;
		(*reverseRenaming)[newItemID] = item;

		(*supportCounts)[newItemID] = support;
		(*distinctTransactionsCounts)[newItemID] = support;

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

	auto new_renaming_size =
			max(olderReverseRenaming->size(), supportCounts->size());
	auto new_renaming = make_p_array_int32(new_renaming_size);
	reverseRenaming = make_p_array_int32(nbFrequents);
	auto opt_supportCounts = supportCounts->array;
	auto opt_new_renaming = new_renaming->array;
	auto opt_reverseRenaming = reverseRenaming->array;
	auto opt_olderReverseRenaming = olderReverseRenaming->array;
	auto opt_distinctTransactionsCounts = distinctTransactionsCounts->array;

	// we will always have newItemID <= item
	int newItemID = 0;
	int greatestBelowMaxCandidate = INT32_MIN;

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
		item++;
	}

	maxCandidate = greatestBelowMaxCandidate + 1;
	fill(	new_renaming->begin() + supportCounts->size(),
			new_renaming->end(),
			-1);
	maxFrequent = newItemID - 1;
	compactedArrays = true;

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
