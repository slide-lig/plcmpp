

#pragma once

#include <vector>
using namespace std;

#include "util/Iterator.hpp"
using namespace util;

namespace internals {

class FrequentIterator;
class FrequentsIterator;

class Counters
{
public:
    int32_t minSupport;
    int32_t transactionsCount;
    int32_t distinctTransactionsCount;
    int32_t distinctTransactionLengthSum;
    vector<int32_t>* supportCounts;
    vector<int32_t>* distinctTransactionsCounts;
    int32_t nbFrequents;

protected:
    vector<int32_t>* closure;
    int32_t maxFrequent;
    vector<int32_t>* reverseRenaming;
    vector<int32_t>* renaming;
    bool compactedArrays;
    int32_t maxCandidate;

private:
    static thread_local FrequentIterator* localFrequentsIterator;

public:
    Counters* clone();
    int32_t getMaxFrequent();
    vector<int32_t>* getRenaming();
    vector<int32_t>* getReverseRenaming();

protected:
    void reuseRenaming(vector<int32_t>* olderReverseRenaming);

public:
    vector<int32_t>* compressRenaming(vector<int32_t>* olderReverseRenaming);
    int32_t getMaxCandidate();
    FrequentsIterator* getExtensionsIterator();
    FrequentsIterator* getLocalFrequentsIterator(int32_t from, int32_t to);


    Counters(int32_t minimumSupport, unique_ptr<Iterator<TransactionReader>> transactions, int32_t extension, vector<int32_t>* ignoredItems, int32_t maxItem);

protected:
    Counters(int32_t minimumSupport, unique_ptr<Iterator<TransactionReader>> transactions);

private:
    Counters(int32_t minSupport, int32_t transactionsCount, int32_t distinctTransactionsCount, int32_t distinctTransactionLengthSum, vector<int32_t>* supportCounts, vector<int32_t>* distinctTransactionsCounts, vector<int32_t>* closure, int32_t nbFrequents, int32_t maxFrequent, vector<int32_t>* reverseRenaming, vector<int32_t>* renaming, bool compactedArrays, int32_t maxCandidate);
};

}
