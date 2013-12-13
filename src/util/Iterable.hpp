/*
 * Iterable.hpp
 *
 *  Created on: 10 déc. 2013
 *      Author: etienne
 */

#pragma once

#include <memory>
using namespace std;
#include "Iterator.hpp"

namespace util {

template <class T>
class Iterable {
public:
	unique_ptr<Iterator<T>> iterator() = 0;
};

}
