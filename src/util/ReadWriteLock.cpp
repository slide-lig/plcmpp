/*
 * ReadWriteLock.cpp
 *
 *  Created on: 16 janv. 2014
 *      Author: etienne
 */

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
