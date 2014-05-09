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


#ifndef READWRITELOCK_HPP_
#define READWRITELOCK_HPP_

#include <atomic>
using namespace std;

namespace util {

class ReadWriteLock {
/*
 * _state = 0: lock is free
 * _state = N: lock is taken by N readers
 * _state = -1: lock is taken by 1 writer
 */

private:
	atomic<int32_t> _state;

public:
	ReadWriteLock();
	void readLock();
	void readUnlock();
	void writeLock();
	void writeUnlock();

protected:
	template <bool writeLock>
	class Guard {
	private:
		ReadWriteLock *_rwl;

	public:
		Guard(ReadWriteLock &rwl) {
			_rwl = &rwl;
			if (writeLock) {
				_rwl->writeLock();
			} else {
				_rwl->readLock();
			}
		}

		~Guard() {
			if (writeLock) {
				_rwl->writeUnlock();
			} else {
				_rwl->readUnlock();
			}
		}
	};

public:
	class WriteGuard : public Guard<true> {
	public:
		WriteGuard(ReadWriteLock &rwl) : Guard(rwl) {};
	};
	class ReadGuard : public Guard<false> {
	public:
		ReadGuard(ReadWriteLock &rwl) : Guard(rwl) {};
	};
};

} /* namespace util */

#endif /* READWRITELOCK_HPP_ */
