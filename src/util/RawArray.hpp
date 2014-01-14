/*
 * RawArray.h
 *
 *  Created on: 14 janv. 2014
 *      Author: etienne
 */

#ifndef RAWARRAY_HPP_
#define RAWARRAY_HPP_

#include <cstring>
using namespace std;

namespace util {

template <class T>
class RawArray {
public:
	T * array;
	uint32_t length;

	inline RawArray(uint32_t in_length, T init_value = 0) {
		length = in_length;
		array = new T[in_length];
		for (auto it = begin(); it != end(); it++)
		{
			*it = init_value;
		}
	}

	inline RawArray(RawArray<T> &other) {
		length = other.length;
		array = new T[length];
		memcpy(array, other.array, length*sizeof(T));
	}

	inline ~RawArray()
	{
		delete[] array;
	}

	inline T* begin() {
		return array;
	}

	inline T* end() {
		return array + length;
	}

	inline uint32_t size() {
		return length;
	}

	inline T& operator[](uint32_t n)
	{
		return *(array + n);
	}
};

} /* namespace util */

#endif /* RAWARRAY_HPP_ */
