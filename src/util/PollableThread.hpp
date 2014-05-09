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

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

namespace util {

class PollableThread
{
private:
	queue<char> _queue;
	mutex _mutex;
	condition_variable _queue_cond_var;
	long _auto_poll_timeout;

	thread* _thread;

	int run();

protected:
	virtual void onInit();
	virtual void onPoll(bool timeout) = 0;
	void sendToThread(char c);

public:
	PollableThread(long auto_poll_timeout);
	virtual ~PollableThread();

	void start();
	void poll();
	void stop();
};

}
