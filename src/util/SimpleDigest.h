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

#include <cstdint>
#include <iostream>
using namespace std;

//#define COMPUTE_DIGEST_STATS

#ifdef COMPUTE_DIGEST_STATS
#define REPORT_DIGEST_STATS() SimpleDigest::report_stats()
#define ADD_DIGEST_FALSE_POSITIVE(a, b, c, d) \
		SimpleDigest::add_false_positive(a, b, c, d)
#else
#define REPORT_DIGEST_STATS()
#define ADD_DIGEST_FALSE_POSITIVE(a, b, c, d)
#endif

/* You can specify a shuffling algorithm at build time by using, for example:
 * export CPPFLAGS='-DSIMPLE_DIGEST_SHUFFLING="d ^= (*begin) << (d%47+1);"'
 * If the hashes are then used many times, it should be valuable to perform
 * an accurate hashing, which will produce few duplicates.
 * Otherwise, the calculation of hashes may be too costly, and you should
 * select a faster (and maybe less accurate) algorithm.
 *  */
#define ACCURATE_SHUFFLING 		d ^= d << (d%47+1); d ^= *begin + d%61 + 1;
#define INTERMEDIATE_SHUFFLING	d ^= (*begin) << (d%47+1);
#define NO_SHUFFLING 			break;

#ifndef SIMPLE_DIGEST_SHUFFLING
#define SIMPLE_DIGEST_SHUFFLING  INTERMEDIATE_SHUFFLING
#endif

namespace util {

#define SEED		0x01030507090b0d0f

class SimpleDigest {

public:
	typedef uint64_t Type;

	template <class T>
	static uint64_t digest(T *begin, T *end)
	{
		uint64_t d = SEED;

		// case of empty range
		if (begin == end)
		{
			return d-1;
		}

		// 1st occurrence, do something simple
		d ^= *begin;
		++begin;

		// next occurrences, we will have to
		// be smarter and shuffle things...
		for (; begin != end; ++begin)
		{
			SIMPLE_DIGEST_SHUFFLING
		}

#ifdef COMPUTE_DIGEST_STATS
		record_hash(d);
#endif
		return d;
	}

	template <class T>
	static void add_false_positive(
			T* t1_start, T* t1_end,
			T* t2_start, T* t2_end)
	{
		cout << "The following 2 integers series returned the same hash (this is a false positive):" << endl;
		cout << "[" ;
		for (auto it = t1_start; it < t1_end; it++)
		{
			cout << " " << (int)*it;
		}
		cout << " ]" << endl;

		cout << "[" ;
		for (auto it = t2_start; it < t2_end; it++)
		{
			cout << " " << (int)*it;
		}
		cout << " ]" << endl;

		increment_false_positives();
	}

	static void record_hash(uint64_t d);
	static void increment_false_positives();
	static void report_stats();
};

} /* namespace util */

