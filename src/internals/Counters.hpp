

#pragma once

#include <vector>
#include <memory>
#include <atomic>
using namespace std;

#include "internals/TransactionReader.hpp"
#include "internals/FrequentsIterator.hpp"
#include "util/Iterator.hpp"
#include "util/shortcuts.h"
using namespace util;

namespace internals {

class FrequentIterator; // defined below

/**
 * This class' constructor performs item counting over a transactions database,
 * and then gives access to various counters. It ignores items below the minimum
 * support. During recursions this allows the algorithm to choose what kind of
 * projections and filtering should be done, before instantiating the actual
 * projected dataset.
 *
 * We're using arrays as Maps<Int,Int> , however they're not public as item
 * identifiers may have been renamed by dataset representation. By the way this
 * class is able to generate renamings (and applies them to itself by the way)
 * if you need to rename items in a future representation. You *MUST* handle
 * renaming after instantiation. See field reverseRenaming.
 */
class Counters
{
public:

	/**
	 * Items occuring less than minSup times will be considered infrequent
	 */
    int32_t minSupport;

	/**
	 * How many transactions are represented by the given dataset ?
	 */
    int32_t transactionsCount;

	/**
	 * How many transactions have been counted (equals transactionsCount when
	 * all transactions have a weight of 1)
	 */
    int32_t distinctTransactionsCount;

	/**
	 * Sum of given *filtered* transactions' lengths, ignoring their weight
	 */
    int32_t distinctTransactionLengthSum;

	/**
	 * Support count, per item having a support count in [minSupport; 100% [
	 * Items having a support count below minSupport are considered infrequent,
	 * those at 100% belong to closure, for both supportCounts[i] = 0 - except
	 * if renaming happened, in which case such items no longer exists.
	 *
	 * Indexes above maxFrequent should be considered valid.
	 */
    shp_vec_int32 supportCounts;

	/**
	 * For each item having a support count in [minSupport; 100% [ , gives how
	 * many distinct transactions contained this item. It's like supportCounts
	 * if all transactions have a weight equal to 1
	 *
	 * Indexes above maxFrequent should be considered valid.
	 */
    shp_vec_int32 distinctTransactionsCounts;

	/**
	 * Counts how many items have a support count in [minSupport; 100% [
	 */
    int32_t nbFrequents;

	/**
	 * will be set to true if arrays have been compacted, ie. if supportCounts
	 * and distinctTransactionsCounts don't contain any zero.
	 */
    bool compactedArrays;

	/**
	 * Biggest item ID having a support count in [minSupport; 100% [
	 */
    int32_t maxFrequent;


protected:
	/**
	 * Items found to have a support count equal to transactionsCount (using IDs
	 * from given transactions) On renamed datasets you SHOULD NOT use
	 * getReverseRenaming to translate back these items, rather use parent's
	 * reverseRenaming (or none for the initial dataset)
	 */
    shp_vec_int32 closure;

	/**
	 * This array allows another class to output the discovered closure using original items' IDs.
	 *
	 * After instanciation this field *must* be set by one of these methods
	 * - reuseRenaming, the initial dataset's constructor (which also sets "renaming")
	 * - compressRenaming, useful when recompacting dataset in recursions
	 */
    shp_vec_int32 reverseRenaming;

	/**
	 * This field will be null EXCEPT if you're using the initial dataset's
	 * constructor (in which case it computes its absolute renaming by the way)
	 * OR if you called compressRenaming (in which case getRenaming will give back the same value)
	 *
	 * It gives, for each original item ID, its new identifier. If it's negative
	 * it means the item should be filtered.
	 */
    shp_vec_int32 renaming;

	/**
	 * Exclusive index of the first item >= core_item in current base
	 */
    int32_t maxCandidate;

private:
	/**
	 * We use our own map, although it will contain a single item most of the time, because
	 * ThreadLocal causes (huge) memory leaks when used as a non-static field.
	 * @see getLocalFrequentsIterator
	 */
    static thread_local FrequentIterator* localFrequentsIterator;

public:
	/**
	 * Does item counting over a projected dataset
	 *
	 * @param minimumSupport
	 * @param transactions
	 *            extension's support
	 * @param extension
	 *            the item on which we're projecting - it won't appear in *any*
	 *            counter (not even 'closure')
	 * @param maxItem
	 *            biggest index among items to be found in "transactions"
	 */
    Counters(int32_t minimumSupport, Iterator<TransactionReader*>* transactions, int32_t extension, int32_t maxItem);
    ~Counters();

protected:
	/**
	 * Does item counting over an initial dataset : it will only ignore
	 * infrequent items, and it doesn't know what's biggest item ID. IT ALSO
	 * IGNORES TRANSACTIONS WEIGHTS ! (assuming it's 1 everywhere) /!\ It will
	 * perform an absolute renaming : items are renamed (and, likely,
	 * re-ordered) by decreasing support count. For instance 0 will be the most
	 * frequent item.
	 *
	 * Indexes in arrays will refer items' new names, except for closure.
	 *
	 * @param minimumSupport
	 * @param transactions
	 */
    Counters(int32_t minimumSupport, Iterator<TransactionReader*>* transactions);

private:
    Counters(const Counters& other);	// copy constructor

public:
    unique_ptr<Counters> clone();

	/**
	 * @return greatest frequent item's ID, which is also the greatest valid
	 *         index for arrays supportCounts and distinctTransactionsCounts
	 */
	int32_t getMaxFrequent();

	/**
	 * @return the renaming map from instantiation's base to current base
	 */
	shp_vec_int32 getRenaming();

	/**
	 * @return a translation from internal item indexes to dataset's original indexes
	 */
	shp_vec_int32 getReverseRenaming();

	/**
	 * Will compress an older renaming, by removing infrequent items. Contained
	 * arrays (except closure) will refer new item IDs
	 *
	 * @param olderReverseRenaming
	 *            reverseRenaming from the dataset that fed this Counter
	 * @return the translation from the old renaming to the compressed one
	 *         (gives -1 for removed items)
	 */
	shp_vec_int32 compressRenaming(shp_vec_int32 olderReverseRenaming);

    int32_t getMaxCandidate();

	/**
	 * Notice: enumerated item IDs are in local base, use this.reverseRenaming
	 *
	 * @return a thread-safe iterator over frequent items (in ascending order)
	 */
    unique_ptr<FrequentsIterator> getExtensionsIterator();

    /**
	 * Notice: enumerated item IDs are in local base, use this.reverseRenaming
	 *
	 * @return an iterator over frequent items (in ascending order)
	 */
    FrequentsIterator* getLocalFrequentsIterator(int32_t from, int32_t to);

protected:
    void reuseRenaming(shp_vec_int32 olderReverseRenaming);

};

/**
 * Thread-safe iterator over frequent items (ie. those having a support
 * count in [minSup, 100%[)
 */
class ExtensionsIterator : public FrequentsIterator
{
private:
    atomic<int32_t> index;
    int32_t max;
    Counters *_counters;

public:
	/**
	 * will provide an iterator on frequent items (in increasing order) in
	 * [0,to[
	 */
    ExtensionsIterator(Counters *Counters, int32_t to);

	/**
	 * @return -1 if iterator is finished
	 */
    int32_t next() override;
    int32_t peek() override;
    int32_t last() override;
};

class FrequentIterator : public FrequentsIterator
{
private:
    int32_t index;
    int32_t max;
    shp_vec_int32 supportsFilter;

public:
    FrequentIterator();

    void recycle(int32_t from, int32_t to, Counters* instance);
    int32_t next() override;
    int32_t peek() override;
    int32_t last() override;
};

}
