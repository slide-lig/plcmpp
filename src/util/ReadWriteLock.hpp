/*
 * ReadWriteLock.hpp
 *
 *  Created on: 16 janv. 2014
 *      Author: etienne
 */

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
