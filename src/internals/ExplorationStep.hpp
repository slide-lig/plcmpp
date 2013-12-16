#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdint>
using namespace std;

#include "internals/FrequentsIterator.hpp"

namespace internals {

// forward declarations

class Dataset;
class TransactionsIterable;
class Counters;
class Selector;


struct Progress {
	public:
		int current;
		int last;

		Progress(FrequentsIterator* candidates);
};


class ExplorationStep
{

private:
    static bool verbose;
    static bool ultraVerbose;

    static const string KEY_VIEW_SUPPORT_THRESHOLD;
    static const string KEY_LONG_TRANSACTIONS_THRESHOLD;

public:
    vector<uint32_t>* pattern;
    int32_t core_item;
    Dataset* dataset;
    Counters* counters;

protected:
    Selector* selectChain;
    FrequentsIterator* candidates;

private:
    map<uint32_t, uint32_t>* failedFPTests;

public:
    ExplorationStep* next();

private:
    Dataset* instanciateDataset(ExplorationStep* parent, TransactionsIterable* support);

public:
    int32_t getFailedFPTest(int32_t item);

private:
    void addFailedFPTest(int32_t item, int32_t firstParent);

public:
    void appendSelector(Selector* s);
    int32_t getCatchedWrongFirstParentCount();
    ExplorationStep* copy();
    Progress* getProgression();

    ExplorationStep(int32_t minimumSupport, string& path);

private:
    ExplorationStep(vector<uint32_t>* pattern, int32_t core_item,
    		Dataset* dataset, Counters* counters, Selector* selectChain,
    		FrequentsIterator* candidates,
    		map<uint32_t, uint32_t>* failedFPTests);

protected:
    ExplorationStep(ExplorationStep* parent, int32_t extension,
    		Counters* candidateCounts, TransactionsIterable* support);
};

}
