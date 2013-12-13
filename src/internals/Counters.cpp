#include <map>
#include <queue>
#include <memory>
#include <algorithm>
using namespace std;

#include <internals/Counters.hpp>
#include <internals/TransactionReader.hpp>
#include <util/ItemAndSupport.hpp>
#include <util/ItemsetsFactory.hpp>
using namespace util;

namespace internals {

struct FrequentIterator* static_frequent_iterator_init() {

	return new FrequentIterator();
}

thread_local FrequentIterator* Counters::localFrequentsIterator =
		static_frequent_iterator_init();

shp_vec_int32 make_p_vec_int32(uint32_t size)
{
	return make_shared<vec_int32>(vec_int32(size));
}

shp_vec_int32 make_p_vec_int32(uint32_t size, int32_t init_value)
{
	return make_shared<vec_int32>(vec_int32(size, init_value));
}

Counters::Counters(
		int32_t minimumSupport,
		Iterator<TransactionReader*>* transactions,
		int32_t extension,
		int32_t maxItem) {

	renaming = 0;
	minSupport = minimumSupport;
	supportCounts = make_p_vec_int32(maxItem + 1);
	distinctTransactionsCounts = make_p_vec_int32(maxItem + 1);
	reverseRenaming = 0;

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
					(*supportCounts)[item] += weight;
					(*distinctTransactionsCounts)[item]++;
				}
			}
		}
	}

	transactionsCount = weightsSum;
	distinctTransactionsCount = transactionsCount;

	// ignored items
	(*supportCounts)[extension] = 0;
	(*distinctTransactionsCounts)[extension] = 0;
	maxCandidate = extension;

	// item filtering and final computations : some are infrequent, some
	// belong to closure

	ItemsetsFactory *closureBuilder = new ItemsetsFactory();
	uint32_t remainingDistinctTransLengths = 0;
	uint32_t remainingFrequents = 0;
	uint32_t biggestItemID = 0;

	for (uint32_t i = 0; i < supportCounts->size(); i++) {
		if ((*supportCounts)[i] < minimumSupport) {
			(*supportCounts)[i] = 0;
			(*distinctTransactionsCounts)[i] = 0;
		} else if ((*supportCounts)[i] == transactionsCount) {
			closureBuilder->push_back(i);
			(*supportCounts)[i] = 0;
			(*distinctTransactionsCounts)[i] = 0;
		} else {
			biggestItemID = max(biggestItemID, i);
			remainingFrequents++;
			remainingDistinctTransLengths += (*distinctTransactionsCounts)[i];
		}
	}

	closure.reset(closureBuilder);
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
	renaming = make_p_vec_int32(biggestItemID + 1, -1);

	// item filtering and final computations : some are infrequent, some
	// belong to closure
	priority_queue<ItemAndSupport*> renamingHeap;
	ItemsetsFactory *closureBuilder = new ItemsetsFactory();

	for (auto iterator = supportsMap->begin(); iterator != supportsMap->end();
			iterator++) {
		int32_t item = iterator->first;
		int32_t supportCount = iterator->second;

		if (supportCount == transactionsCount) {
			closureBuilder->push_back(item);
		} else if (supportCount >= minimumSupport) {
			renamingHeap.push(new ItemAndSupport(item, supportCount));
		} // otherwise item is infrequent : its renaming is already -1, ciao
	}

	closure.reset(closureBuilder);
	nbFrequents = renamingHeap.size();
	maxFrequent = nbFrequents - 1;
	maxCandidate = maxFrequent + 1;

	supportCounts = make_p_vec_int32(nbFrequents);
	distinctTransactionsCounts = make_p_vec_int32(nbFrequents);
	reverseRenaming = make_p_vec_int32(nbFrequents);
	int remainingSupportsSum = 0;

	int newItemID = 0;

	while (!renamingHeap.empty()) {
		ItemAndSupport *entry = renamingHeap.top();
		int32_t item = entry->item();
		int32_t support = entry->support();

		renamingHeap.pop();
		delete entry;

		(*renaming)[item] = newItemID;
		(*reverseRenaming)[newItemID] = item;

		(*supportCounts)[newItemID] = support;
		(*distinctTransactionsCounts)[newItemID] = support;

		remainingSupportsSum += support;

		newItemID++;
	}

	compactedArrays = true;
	distinctTransactionLengthSum = remainingSupportsSum;

	delete supportsMap;
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

Counters::~Counters() {
}

unique_ptr<Counters> Counters::clone() {
	// create a clone using the copy constructor
	Counters *cloned = new Counters(*this);
	return unique_ptr<Counters>(cloned);
}

int32_t Counters::getMaxFrequent() {
	return maxFrequent;
}

shp_vec_int32 Counters::getRenaming() {
	return renaming;
}

shp_vec_int32 Counters::getReverseRenaming() {
	return reverseRenaming;
}

void Counters::reuseRenaming(shp_vec_int32 olderReverseRenaming) {
	reverseRenaming = olderReverseRenaming;
}

shp_vec_int32 Counters::compressRenaming(
		shp_vec_int32 olderReverseRenaming) {

	auto new_renaming_size =
			max(olderReverseRenaming->size(), supportCounts->size());
	auto new_renaming = make_p_vec_int32(new_renaming_size);
	reverseRenaming = make_p_vec_int32(nbFrequents);

	// we will always have newItemID <= item
	int newItemID = 0;
	int greatestBelowMaxCandidate = INT32_MIN;

	for (int32_t item = 0; item < (int32_t)supportCounts->size(); item++) {
		if ((*supportCounts)[item] > 0) {
			(*new_renaming)[item] = newItemID;
			(*reverseRenaming)[newItemID] = (*olderReverseRenaming)[item];

			(*distinctTransactionsCounts)[newItemID] = (*distinctTransactionsCounts)[item];
			(*supportCounts)[newItemID] = (*supportCounts)[item];

			if (item < maxCandidate) {
				greatestBelowMaxCandidate = newItemID;
			}

			newItemID++;
		} else {
			(*new_renaming)[item] = -1;
		}
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

FrequentsIterator* Counters::getLocalFrequentsIterator(int32_t from,
		int32_t to) {
	FrequentIterator *iterator = localFrequentsIterator;
	iterator->recycle(from, to, this);
	return iterator;
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
		while (true) {
			int32_t nextIndex = index++;
			if (nextIndex < max) {
				if ((*_counters->supportCounts)[nextIndex] > 0) {
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

FrequentIterator::FrequentIterator() {
	max = 0;
	index = 0;
	supportsFilter = nullptr;
}

void FrequentIterator::recycle(int32_t from, int32_t to, Counters* instance) {
	max = to;
	index = from;
	supportsFilter = instance->compactedArrays ? nullptr :
			instance->supportCounts;
}

// TODO: factorize w.r.t. ExtensionsIterator::next()

int32_t FrequentIterator::next() {
	if (supportsFilter == nullptr) {
		int32_t nextIndex = index++;
		if (nextIndex < max) {
			return nextIndex;
		} else {
			return -1;
		}
	} else {
		while (true) {
			int32_t nextIndex = index++;
			if (nextIndex < max) {
				if ((*supportsFilter)[nextIndex] > 0) {
					return nextIndex;
				}
			} else {
				return -1;
			}
		}
	}
	return 0;	// never reached but avoids a warning
}

int32_t FrequentIterator::peek() {
	return index;
}

int32_t FrequentIterator::last() {
	return max;
}

}
