/*
 * Helpers.h
 *
 *  Created on: 17 déc. 2013
 *      Author: etienne
 */

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;

namespace util {

class Helpers {
public:
	static string formatted_time();

	template <class T>
	static string printed_vector(vector<T> *vec) {
	  std::ostringstream oss;

	  oss << "[ ";

	  if (!vec->empty())
	  {
		// Convert all but the last element to avoid a trailing ","
		std::copy(vec->begin(), vec->end()-1,
			std::ostream_iterator<T>(oss, ","));

		// Now add the last element with no delimiter
		oss << vec->back();
	  }

	  oss << " ]";

	  return string(oss.str());
	}
};

} /* namespace util */

