
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
	_array = new vector<T>(_storage_size);
}

template <class T>
Template_ConsecutiveItemsConcatenatedTidList<T>::Template_ConsecutiveItemsConcatenatedTidList(
		const Template_ConsecutiveItemsConcatenatedTidList<T>& other) :
		ConsecutiveItemsConcatenatedTidList(other) {
	_array = new vector<T>(*(other._array));
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
int32_t Template_ConsecutiveItemsConcatenatedTidList<T>::read(
		int32_t position) {
	return (*_array)[position];
}

template <class T>
void Template_ConsecutiveItemsConcatenatedTidList<T>::write(
		int32_t position, int32_t transaction) {
	if (transaction > MAX_VALUE) {
		cerr << transaction <<
				" too big for this kind of tid-list! Aborting."
				<< endl;
		abort();
	}
	(*_array)[position] = (T) transaction;
}

template <class T>
bool Template_ConsecutiveItemsConcatenatedTidList<T>::compatible(
		int32_t maxTid) {
	return maxTid <= MAX_VALUE;
}

}
}
