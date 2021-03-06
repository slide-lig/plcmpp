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

#include <iostream>
using namespace std;

#include <internals/tidlist/Template_ConsecutiveItemsConcatenatedTidList.hpp>

namespace internals {
namespace tidlist {

template <class T>
const T Template_ConsecutiveItemsConcatenatedTidList<T>::MAX_VALUE = ~((T)0);

template <class T>
Template_ConsecutiveItemsConcatenatedTidList<T>::Template_ConsecutiveItemsConcatenatedTidList(
		Counters* c, int32_t highestItem) : ConsecutiveItemsConcatenatedTidList(
				c, highestItem) {
	_array = new RawArray<T>(_storage_size);
	_array_fast = _array->array;
}

template <class T>
Template_ConsecutiveItemsConcatenatedTidList<T>::Template_ConsecutiveItemsConcatenatedTidList(
		const Template_ConsecutiveItemsConcatenatedTidList<T>& other) :
		ConsecutiveItemsConcatenatedTidList(other) {
	_array = new RawArray<T>(*(other._array));
	_array_fast = _array->array;
}

template <class T>
Template_ConsecutiveItemsConcatenatedTidList<T>::~Template_ConsecutiveItemsConcatenatedTidList()
{
	delete _array;
}

template <class T>
unique_ptr<TidList> Template_ConsecutiveItemsConcatenatedTidList<T>::clone() {
	// create a clone using the copy constructor
	Template_ConsecutiveItemsConcatenatedTidList<T> *cloned = new
			Template_ConsecutiveItemsConcatenatedTidList<T>(*this);
	return unique_ptr<TidList>(cloned);
}

template <class T>
void Template_ConsecutiveItemsConcatenatedTidList<T>::write(
		int32_t position, int32_t transaction) {
	if ((uint32_t)transaction > MAX_VALUE) {
		cerr << transaction <<
				" too big for this kind of tid-list! Aborting."
				<< endl;
		abort();
	}
	_array_fast[position] = (T) transaction;
}

template <class T>
unique_ptr<TidList::ItemTidList>
Template_ConsecutiveItemsConcatenatedTidList<T>::getItemTidList(
		int32_t item) {

	uint32_t itemIndex = item << 1;
	if (itemIndex > _indexAndFreqs_size ||
			_indexAndFreqs_fast[itemIndex] == -1) {
		cerr << "item " << item << " has no tidlist" << endl;
		abort();
	}

	T* startPos = _array_fast + _indexAndFreqs_fast[itemIndex];
	T* endPos = startPos + (_indexAndFreqs_fast[itemIndex + 1]);
	return unique_ptr<TidList::ItemTidList>(
			new Template_ItemTidList<T>(startPos, endPos));
}

/* check if tidlist of item_a is included in
 * tidlist of item_b */
template <class T>
bool Template_ConsecutiveItemsConcatenatedTidList<T>::checkTidListsInclusion(
		int32_t item_a, int32_t item_b)
{
	int32_t* info_a = _indexAndFreqs_fast + (item_a << 1);
	int32_t* info_b = _indexAndFreqs_fast + (item_b << 1);
	T* start_a = _array_fast + (*info_a);
	T* end_a = start_a + (*(info_a+1));
	T* start_b = _array_fast + (*info_b);
	T* end_b = start_b + (*(info_b+1));

	return std::includes(start_b, end_b, start_a, end_a);
}

template<class T>
internals::tidlist::Template_ItemTidList<T>::Template_ItemTidList(
		T* in_begin, T* in_end) : begin(in_begin), end(in_end) {
}

template<class T>
unique_ptr<Iterator<int32_t> > internals::tidlist::Template_ItemTidList<T>::iterator() {
	return unique_ptr<Iterator<int32_t> >(
			new Template_TidIterator<T>(begin, end));
}

template<class T>
Template_TidIterator<T>::Template_TidIterator(
		T* in_begin, T* in_end) : pos(in_begin), end(in_end)  {
}

template<class T>
bool internals::tidlist::Template_TidIterator<T>::hasNext() {
	return pos != end;
}

template<class T>
int32_t internals::tidlist::Template_TidIterator<T>::next() {
	return *(pos++);
}

template <class T>
bool Template_ConsecutiveItemsConcatenatedTidList<T>::compatible(
		int32_t maxTid) {
	return maxTid <= MAX_VALUE;
}

}
}
