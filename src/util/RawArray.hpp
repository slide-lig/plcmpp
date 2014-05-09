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
