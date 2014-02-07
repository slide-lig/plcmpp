/*
 * SimpleDigest.cpp
 *
 *  Created on: 5 févr. 2014
 *      Author: etienne
 */

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

