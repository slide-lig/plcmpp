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
#include <sys/resource.h>

namespace util {
namespace MemoryUsage {

class WatcherThread : public PollableThread
{
private:
	uint _kbytes_per_page;

	void onInit() override;
	void onPoll(bool timeout) override;
	void printRSS();

public:
	WatcherThread();
	~WatcherThread();
};

long getPeakMemoryUsage();

}
}
