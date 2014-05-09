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

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <memory>
#include <iterator>
using namespace std;

namespace util {

class Helpers {
public:
	static string formatted_time();
	static double precise_time();

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

	template <class T>
	static shared_ptr<T> unique_to_shared(unique_ptr<T> up) {
		// release the ownership, then make it shared
		return shared_ptr<T>(up.release());
	}
};

} /* namespace util */

