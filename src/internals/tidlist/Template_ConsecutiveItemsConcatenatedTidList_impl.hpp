
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
	if (transaction > (int32_t)MAX_VALUE) {
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
