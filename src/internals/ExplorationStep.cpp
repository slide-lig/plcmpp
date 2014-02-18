#include <climits>
#include <iostream>
using namespace std;

#include <internals/ExplorationStep.hpp>
#include <internals/Counters.hpp>
#include <internals/Dataset.hpp>
#include <internals/FirstParentTest.hpp>
#include <internals/FrequentsIterator.hpp>
#include <internals/Selector.hpp>
#include <io/FileReader.hpp>
#include <util/ItemsetsFactory.hpp>
#include "util/Helpers.h"

using io::FileReader;
using util::Helpers;

namespace internals {

bool ExplorationStep::verbose = false;
bool ExplorationStep::ultraVerbose = false;

Selector *ExplorationStep::firstParentTestInstance = new FirstParentTest();

ExplorationStep::ExplorationStep(int32_t minimumSupport,
		string& path) : candidates(nullptr) {
	core_item = INT_MAX;
	selector = nullptr;
	/* the following FileReader will just be used in the constructors of
	 * Counters and Dataset, so we declare it local in this
	 * constructor.
	 */
	FileReader reader(path);

	// reading of the file occurs in the constructor of Counters:
	counters = unique_ptr<Counters>(new Counters(minimumSupport, &reader));

	// transactions were saved in memory while reading the file.
	// we retrieve them now and specify a renaming.
	auto savedTransactions = reader.getSavedTransactions();

	pattern = counters->getClosure();

	dataset = DatasetFactory::initFirstDataset(
			savedTransactions.get(),
			counters->getRenaming(),
			counters.get(),
			-1 /* no core item yet */
			);

	candidates = Helpers::unique_to_shared(counters->getExtensionsIterator());

	failedFPTests.reset(new unordered_map<int32_t, int32_t>());
}

ExplorationStep::ExplorationStep(ExplorationStep* parent,
		int32_t extension, unique_ptr<Counters> candidateCounts) {
	core_item = extension;
	counters = move(candidateCounts); // get ownership
	shp_array_int32 reverseRenaming = parent->counters->getReverseRenaming();

	if (verbose) {
		if (parent->pattern->size() == 0 || ultraVerbose) {
			cerr << "{\"time\":\"" << Helpers::formatted_time() <<
					"\",\"thread\":" << PLCM::getCurrentThread()->getHumanReadableId() <<
					",\"pattern\":" <<
							Helpers::printed_vector(parent->pattern.get()) <<
					",\"extension_internal\":" << extension <<
					",\"extension\":" << (*reverseRenaming)[extension] << "}" << endl;
		}
	}

	pattern.reset(new vec_int32());
	ItemsetsFactory::extendRename(
			*(counters->getClosure()), extension, *(parent->pattern), *(reverseRenaming),
			*(pattern));

	if (counters->nbFrequents == 0 || counters->distinctTransactionsCount == 0) {
		candidates = nullptr;
		failedFPTests = nullptr;
		selector = nullptr;
		dataset = nullptr;
	} else {
		failedFPTests.reset(
				new unordered_map<int32_t, int32_t>());

		selector = ExplorationStep::firstParentTestInstance;

		// indeed, instantiateDataset is influenced by longTransactionsMode
		dataset = instanciateDataset(parent, extension);

		// and intanciateDataset may choose to trigger some renaming in
		// counters
		candidates = Helpers::unique_to_shared(counters->getExtensionsIterator());
	}
}

unique_ptr<ExplorationStep> ExplorationStep::next() {

	if (candidates == nullptr) {
		return nullptr;
	}

	while (true) {
		int32_t candidate = candidates->next();

		if (candidate < 0) {
			return nullptr;
		}

		if (selector == nullptr || selector->select(candidate, this)) {
			unique_ptr<TransactionsSubList> item_transactions = dataset->getTransactionsSubList(
					candidate);

			unique_ptr<Counters> candidateCounts(
					new Counters(counters->minSupport, item_transactions.get(), candidate,
							counters->maxFrequent));

			auto closure = candidateCounts->getClosure().get();
			if (closure->size() > 0)
			{
				int32_t greatest = *std::max_element(closure->begin(), closure->end());

				if (greatest > candidate) {
					addFailedFPTest(candidate, greatest);
					continue;
				}
			}
			// instanciateDataset may choose to compress renaming - if
			// not, at least it's set for now.
			candidateCounts->reuseRenaming(counters->getReverseRenaming());

			return unique_ptr<ExplorationStep>(
					new ExplorationStep(
							this, candidate, move(candidateCounts) /* transfer ownership */));
		}
	}

	return nullptr; // never reached, here to avoid a warning
}

unique_ptr<Dataset> ExplorationStep::instanciateDataset(ExplorationStep* parent,
				int32_t extension) {

	shp_array_int32 renaming = counters->compressRenaming(
				parent->counters->getReverseRenaming());

	return parent->dataset->instanciateChildDataset(
			extension,
			renaming,
			counters.get(),
			counters->getMaxCandidate()
			);
}

void ExplorationStep::addFailedFPTest(int32_t item,
		int32_t firstParent) {
	unique_lock<mutex> lock(failedFPTests_mutex);
	(*failedFPTests)[item] = firstParent;
}

int32_t ExplorationStep::getCatchedWrongFirstParentCount() {
	if (failedFPTests == nullptr) {
		return 0;
	} else {
		return failedFPTests->size();
	}
}

unique_ptr<Progress> ExplorationStep::getProgression() {
	return unique_ptr<Progress>(new Progress(candidates.get()));
}

Progress::Progress(FrequentsIterator *candidates) {
	current = candidates->peek();
	last = candidates->last();

}

}
