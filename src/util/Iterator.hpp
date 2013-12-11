/*
 * Iterable.h
 *
 *  Created on: 10 déc. 2013
 *      Author: etienne
 */

#pragma once

namespace util {

template <class T>
class Iterator {
public:
	virtual ~Iterator() {};
	virtual bool hasNext() = 0;
	virtual T& next() = 0;
};

}
