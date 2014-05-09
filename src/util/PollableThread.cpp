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

#include <chrono>
#include <iostream>
#include <unistd.h>
#include <cstring>

#include "util/PollableThread.hpp"

using namespace std;
using std::chrono::system_clock;
using std::chrono::milliseconds;

static char POLL_REQUEST = 'P';
static char INTERRUPT_REQUEST = 'I';
static char POLL_TIMEOUT = 'T';

namespace util {

PollableThread::PollableThread(
		long auto_poll_timeout) {
	_thread = 0;
	_auto_poll_timeout = auto_poll_timeout;
}

PollableThread::~PollableThread() {
	if (_thread != 0){
		cout << "Deleting thread." << endl;
		cout.flush();
		delete _thread;
	}
}

int PollableThread::run() {

	onInit();

	while (true)
	{
		char request;

		auto timeout_date = system_clock::now() + milliseconds(_auto_poll_timeout);

		{
			unique_lock<mutex> ul(_mutex);
			while (true)
			{
				auto status = _queue_cond_var.wait_until(ul, timeout_date);

				if (status == cv_status::timeout)
					break;

				if (!_queue.empty())
					break; // otherwise, loop again, it was a spurious wake-up
			}
			if (_queue.empty())
			{
				request = POLL_TIMEOUT;
			}
			else
			{
				request = _queue.front();
				_queue.pop();
			}
		}

		if (request == INTERRUPT_REQUEST)
		{
			cout << "Thread got interruption request." << endl;
			break;
		}
		else
		{	// POLL_REQUEST or POLL_TIMEOUT
			onPoll(request == POLL_TIMEOUT);
		}
	}

	return 0;
}

void PollableThread::start() {
	// start a thread
	_thread = new thread(&PollableThread::run, this);
}

void PollableThread::sendToThread(char c) {
	{
		lock_guard<mutex> lg(_mutex);
		_queue.push(c);
	}
	_queue_cond_var.notify_one();
}

void PollableThread::stop() {
	sendToThread(INTERRUPT_REQUEST);
	_thread->join();
	delete _thread;
	_thread = 0;
}

void PollableThread::poll() {
	sendToThread(POLL_REQUEST);
}

void PollableThread::onInit() {
	// default is to do nothing
}

}
