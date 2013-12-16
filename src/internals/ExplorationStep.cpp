#include <internals/ExplorationStep.hpp>
#include <internals/Counters.hpp>
#include <internals/Dataset.hpp>
#include <internals/FirstParentTest.hpp>
#include <internals/FrequentsIterator.hpp>
#include <internals/Selector.hpp>
#include <internals/TransactionsRenamingDecorator.hpp>
#include <io/FileReader.hpp>
#include <util/ItemsetsFactory.hpp>

namespace internals {

Progress::Progress(FrequentsIterator* candidates) {
}

ExplorationStep* ExplorationStep::next() {
}

Dataset* ExplorationStep::instanciateDataset(ExplorationStep* parent,
		TransactionsIterable* support) {
}

int32_t ExplorationStep::getFailedFPTest(int32_t item) {
}

void ExplorationStep::addFailedFPTest(int32_t item,
		int32_t firstParent) {
}

void ExplorationStep::appendSelector(Selector* s) {
}

int32_t ExplorationStep::getCatchedWrongFirstParentCount() {
}

ExplorationStep* ExplorationStep::copy() {
}

Progress* ExplorationStep::getProgression() {
}

ExplorationStep::ExplorationStep(int32_t minimumSupport,
		string& path) {
}

ExplorationStep::ExplorationStep(vector<uint32_t>* pattern,
		int32_t core_item, Dataset* dataset, Counters* counters,
		Selector* selectChain, FrequentsIterator* candidates,
		map<uint32_t, uint32_t>* failedFPTests) {
}

ExplorationStep::ExplorationStep(ExplorationStep* parent,
		int32_t extension, Counters* candidateCounts,
		TransactionsIterable* support) {
}

}
