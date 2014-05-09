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
#pragma once

#include <util/PollableThread.hpp>
#include <atomic>

namespace internals {
	class ExplorationStep;
}

using internals::ExplorationStep;

/**
 * This thread will give some information about the progression on stderr every
 * 5 minutes. When running on Hadoop it may also be used to poke the master node
 * every 5 minutes so it doesn't kill the task.
 *
 * you MUST use setInitState before starting the thread you MAY use
 * setHadoopContext
 */

namespace util {

class ProgressWatcherThread : public PollableThread
{
private:
	static const long PRINT_STATUS_EVERY;
	shared_ptr<ExplorationStep> _step;

	void onPoll(bool timeout);

public:
	ProgressWatcherThread();
	void setInitState(shared_ptr<ExplorationStep> step);
};

}
