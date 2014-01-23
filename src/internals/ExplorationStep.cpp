#include <climits>
#include <iostream>
using namespace std;

#include <internals/ExplorationStep.hpp>
#include <internals/Counters.hpp>
#include <internals/Dataset.hpp>
#include <internals/FirstParentTest.hpp>
#include <internals/FrequentsIterator.hpp>
#include <internals/Selector.hpp>
#include <internals/TransactionsRenamingDecorator.hpp>
#include <io/FileReader.hpp>
#include <util/ItemsetsFactory.hpp>
#include "util/Helpers.h"

using io::FileReader;
using util::Helpers;

namespace internals {

bool ExplorationStep::verbose = false;
bool ExplorationStep::ultraVerbose = false;

const string KEY_VIEW_SUPPORT_THRESHOLD = "toplcm.threshold.view";;
const string KEY_LONG_TRANSACTIONS_THRESHOLD = "toplcm.threshold.long";

ExplorationStep::ExplorationStep(int32_t minimumSupport,
		string& path) : candidates(nullptr) {
	core_item = INT_MAX;
	selectChain.reset(new Selector::List());

	/* the following FileReader will just be used in the constructors of
	 * Counters and Dataset, so we declare it local in this
	 * constructor.
	 */
	FileReader reader(path);
	counters = unique_ptr<Counters>(new Counters(minimumSupport, &reader));
	reader.close(counters->getRenaming().get());

	pattern = counters->getClosure();

	dataset = unique_ptr<Dataset>(new Dataset(counters.get(), &reader));

	candidates = Helpers::unique_to_shared(counters->getExtensionsIterator());

	failedFPTests.reset(new map<int32_t, int32_t>());
}

ExplorationStep::ExplorationStep(const ExplorationStep& other) {

	pattern = other.pattern;
	core_item = other.core_item;
	dataset = other.dataset->clone();
	counters = other.counters->clone();
	selectChain = other.selectChain;
	candidates = other.candidates;
	failedFPTests = other.failedFPTests;
}



ExplorationStep::ExplorationStep(ExplorationStep* parent,
		int32_t extension, unique_ptr<Counters> candidateCounts,
		TransactionsIterable* support) {
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
		selectChain = nullptr;
		dataset = nullptr;
	} else {
		failedFPTests.reset(
				new map<int32_t, int32_t>());

		if (parent->selectChain == nullptr) {
			selectChain = nullptr;
		} else {
			selectChain = Helpers::unique_to_shared(
					parent->selectChain->copy());
		}

		selectChain->push_front(unique_ptr<Selector>(new FirstParentTest()));

		// indeed, instantiateDataset is influenced by longTransactionsMode
		dataset = instanciateDataset(parent, support);

		// and intanciateDataset may choose to trigger some renaming in
		// counters
		candidates = Helpers::unique_to_shared(counters->getExtensionsIterator());
	}
}

unique_ptr<ExplorationStep> ExplorationStep::copy() {
	// use the copy constructor
	return unique_ptr<ExplorationStep>(
			new ExplorationStep(*this));
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

		try {
			if (selectChain == nullptr || selectChain->select(candidate, this)) {
				unique_ptr<TransactionsIterable> support = dataset->getSupport(candidate);

				auto it = support->iterator();
				unique_ptr<Counters> candidateCounts(new Counters(
						counters->minSupport,
						it.get(),
						candidate,
						counters->maxFrequent));

				int32_t greatest = INT_MIN;
				shp_vec_int32 closure = candidateCounts->getClosure();
				for (uint32_t i = 0; i < closure->size(); i++) {
					if ((*closure)[i] > greatest) {
						greatest = (*closure)[i];
					}
				}

				if (greatest > candidate) {
					throw Selector::WrongFirstParentException(candidate, greatest);
				}

				// instanciateDataset may choose to compress renaming - if
				// not, at least it's set for now.
				candidateCounts->reuseRenaming(counters->getReverseRenaming());

				return unique_ptr<ExplorationStep>(
						new ExplorationStep(
								this,
								candidate,
								move(candidateCounts),	/* transfer ownership */
								support.get()));
			}
		} catch (Selector::WrongFirstParentException &e) {
			addFailedFPTest(e.extension, e.firstParent);
		}
	}

	return nullptr; // never reached, here to avoid a warning
}

unique_ptr<Dataset> ExplorationStep::instanciateDataset(ExplorationStep* parent,
		TransactionsIterable* support) {
	shp_array_int32 renaming = counters->compressRenaming(
				parent->counters->getReverseRenaming());
	auto it = support->iterator();

	auto filtered = unique_ptr<TransactionsRenamingDecorator>(
					new TransactionsRenamingDecorator(it.get(), renaming));

	Dataset* dataset = new Dataset(
			counters.get(), filtered.get(), INT_MAX);
	dataset->compress(core_item);
	return unique_ptr<Dataset>(dataset);
}

int32_t ExplorationStep::getFailedFPTest(int32_t item) {
	unique_lock<mutex> lock(failedFPTests_mutex);
	return (*failedFPTests)[item];
}

void ExplorationStep::addFailedFPTest(int32_t item,
		int32_t firstParent) {
	unique_lock<mutex> lock(failedFPTests_mutex);
	(*failedFPTests)[item] = firstParent;
}

void ExplorationStep::appendSelector(Selector* s) {
	selectChain->push_back(unique_ptr<Selector>(s));
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
