#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <mutex>
using namespace std;

#include "internals/FrequentsIterator.hpp"
#include "internals/Selector.hpp"
#include "util/shortcuts.h"

using util::shp_vec_int32;

#if defined(PRINT_STEPS) || defined(DEBUG_STEP)
#define RECORD_STEP_ID
#endif

namespace internals {

// forward declarations

class Dataset;
class Counters;
class TransactionsSubList;

struct Progress {
public:
	int current;
	int last;

	Progress(FrequentsIterator *candidates);
};

/**
 * Represents an LCM recursion step. Its also acts as a Dataset factory.
 */
class ExplorationStep {

private:
	static Selector *firstParentTestInstance;

public:
	static bool verbose;
	static bool ultraVerbose;

	/**
	 * closure of parent's pattern UNION extension
	 */
	shp_vec_int32 pattern;
	/**
	 * Extension item that led to this recursion step. Already included in
	 * "pattern".
	 */
	int32_t core_item;
	unique_ptr<Dataset> dataset;
	unique_ptr<Counters> counters;

#ifdef RECORD_STEP_ID
	uint id;
	static uint next_id;
#endif

protected:
	/**
	 * Selector
	 */
	Selector *selector;
	shared_ptr<FrequentsIterator> candidates = nullptr;

private:
	/**
	 * When an extension fails first-parent test, it ends up in this map. Keys
	 * are non-first-parent items associated to their actual first parent.
	 */
	shared_ptr<unordered_map<int32_t, int32_t> > failedFPTests;
	mutex failedFPTests_mutex;

public:
	/**
	 * Finds an extension for current pattern in current dataset and returns the
	 * corresponding ExplorationStep (extensions are enumerated by ascending
	 * item IDs - in internal rebasing) Returns null when all valid extensions
	 * have been generated
	 */
	unique_ptr<ExplorationStep> next();

private:
	unique_ptr<Dataset> instanciateDataset(ExplorationStep* parent,
							int32_t extension);

public:
	void addFailedFPTest(int32_t item, int32_t firstParent);

public:
	int32_t getCatchedWrongFirstParentCount();
	unique_ptr<Progress> getProgression();

	/**
	 * Start exploration on a dataset contained in a file.
	 *
	 * @param minimumSupport
	 * @param path
	 *            to an input file in ASCII format. Each line should be a
	 *            transaction containing space-separated item IDs.
	 */
	ExplorationStep(int32_t minimumSupport, string& path);

protected:
	/**
	 * Instantiate state for a valid extension.
	 *
	 * @param parent
	 * @param extension
	 *            a first-parent extension from parent step
	 * @param candidateCounts
	 *            extension's counters from parent step
	 * @param support
	 *            previously-computed extension's support
	 */
	ExplorationStep(ExplorationStep* parent, int32_t extension,
			unique_ptr<Counters> candidateCounts);
};

}
