/*******************************************************************************
 * Copyright (c) 2014 Etienne Dublé, Martin Kirchgessner, Vincent Leroy, Alexandre Termier, CNRS and Université Joseph Fourier.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *  
 * or see the LICENSE.txt file joined with this program.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

#include <internals/FirstParentTest.hpp>
#include <internals/ExplorationStep.hpp>
#include <internals/Counters.hpp>
#include <internals/Dataset.hpp>
#include <util/shortcuts.h>

using namespace util;

namespace internals {

/**
 * check if we should explore this extension
 */
bool FirstParentTest::allowExploration(int32_t extension,
		ExplorationStep* state)
{
	auto supportCounts = state->counters->supportCounts->array;
	Dataset* d = state->dataset.get();

	int32_t candidateSupport = supportCounts[extension];
	int32_t i = state->counters->maxFrequent;
	supportCounts += i;

	for (; i > extension; i--, supportCounts--) {
		if (*supportCounts >= candidateSupport) {
			if (d->checkTidListsInclusion(extension, i))
			{
				PLCM::getCurrentThread()->counters[
				            PLCM::PLCMCounters::FirstParentTestRejections]++;
				state->addFailedFPTest(extension, i);
				return false;
			}
		}
	}

	return true;
}

}

