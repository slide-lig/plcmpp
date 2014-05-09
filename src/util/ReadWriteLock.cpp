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
#include <thread>
using namespace std;

#include <util/ReadWriteLock.hpp>

inline void wait_before_retrying_lock() {
	this_thread::sleep_for(chrono::nanoseconds(1));
}

namespace util {

ReadWriteLock::ReadWriteLock() {
	_state = 0;
}

void ReadWriteLock::readLock() {

	int32_t expected = _state;

	while (true)
	{
		if(expected == -1)
		{	// lock is hold for writing, this could take time
			wait_before_retrying_lock();
			expected = _state;
		}
		else
		{
			if (_state.compare_exchange_weak(expected, expected+1))
			{
				break;
			}
		}
	}
}

void ReadWriteLock::readUnlock() {
	_state -= 1;	// one reader less
}

void ReadWriteLock::writeLock() {
	int32_t expected = _state;

	while (true)
	{
		if(expected != 0)
		{	// lock is hold for reading or writing, this could take time
			wait_before_retrying_lock();
			expected = _state;
		}
		else
		{
			if (_state.compare_exchange_weak(expected, -1))
			{
				break;
			}
		}
	}
}

void ReadWriteLock::writeUnlock() {
	_state = 0;		// lock is free
}

} /* namespace util */
