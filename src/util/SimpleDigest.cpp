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

#include "SimpleDigest.h"
#include <unordered_set>
#include <atomic>
#include <mutex>
using namespace std;

#ifdef COMPUTE_DIGEST_STATS
static unordered_set<uint64_t> hashes;
static mutex hashes_mutex;
static atomic<uint> false_positives;
#endif

namespace util {

	void SimpleDigest::report_stats()
	{
#ifdef COMPUTE_DIGEST_STATS
		cout << "num hashes: " << hashes.size() << endl;
		cout << "false positives: " << false_positives << endl;
		cerr << "Warning: disable these digest stats for faster execution." << endl;
#endif
	}

	void SimpleDigest::record_hash(uint64_t d) {
#ifdef COMPUTE_DIGEST_STATS
		hashes_mutex.lock();
		hashes.insert(d);
		hashes_mutex.unlock();
#endif
	}

	void SimpleDigest::increment_false_positives() {
#ifdef COMPUTE_DIGEST_STATS
		++false_positives;
#endif
	}

} /* namespace util */

