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

	RawArray(uint32_t in_length) {
		length = in_length;
		array = new T[in_length];
	}

	RawArray(uint32_t in_length, T init_value) :
				RawArray(in_length) {
		auto the_end = end();
		for (auto it = begin(); it != the_end; it++)
		{
			*it = init_value;
		}
	}

	RawArray(RawArray<T> &other) {
		length = other.length;
		array = new T[length];
		memcpy(array, other.array, length*sizeof(T));
	}

	~RawArray()
	{
		delete[] array;
	}

	T* begin() {
		return array;
	}

	T* end() {
		return array + length;
	}

	uint32_t size() {
		return length;
	}

	T& operator[](uint32_t n)
	{
		return *(array + n);
	}
};

} /* namespace util */

#endif /* RAWARRAY_HPP_ */
