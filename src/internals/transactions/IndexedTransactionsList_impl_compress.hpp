
#include <iostream>
#include <unordered_set>
#include <tuple>
using namespace std;

#include <internals/transactions/IndexedTransactionsList.hpp>
#include <internals/Counters.hpp>
#include <util/SimpleDigest.h>

namespace internals {
namespace transactions {

#define TID(prefix_info) 			(std::get<0>(prefix_info))
#define TRANS_START(prefix_info) 	(std::get<1>(prefix_info)->start_transaction)
#define TRANS_END(prefix_info) 		(std::get<1>(prefix_info)->end_transaction)
#define PREFIX_END(prefix_info) 	(std::get<2>(prefix_info))

template <class prefix_info_t>
struct prefix_hasher {
	size_t operator() (const prefix_info_t& prefix_info) const
	{
		return SimpleDigest::digest(
				TRANS_START(prefix_info),
				PREFIX_END(prefix_info));
	}
};

template <class prefix_info_t>
struct prefixes_test_equal {
	bool operator() (
			const prefix_info_t& prefix1_info,
			const prefix_info_t& prefix2_info) const
	{
		// compare prefix length
		if (	(PREFIX_END(prefix2_info) - TRANS_START(prefix2_info)) !=
				(PREFIX_END(prefix1_info) - TRANS_START(prefix1_info)))
			return false;

		// compare prefix
		int cmp = std::memcmp(TRANS_START(prefix1_info), TRANS_START(prefix2_info),
				(PREFIX_END(prefix1_info) - TRANS_START(prefix1_info)) *
					sizeof(*TRANS_START(prefix1_info)));

		if (cmp != 0)
		{
			ADD_DIGEST_FALSE_POSITIVE(TRANS_START(prefix1_info), PREFIX_END(prefix1_info),
					TRANS_START(prefix2_info), PREFIX_END(prefix2_info));
			return false;
		}

		return true; // same prefix
	}
};

typedef int32_t tid_t;

template <class childItemT>
struct PrefixDeduplication {

	typedef IndexedTransactionsList<childItemT> tlist_t;

	typedef tuple<
				tid_t,
				typename tlist_t::transaction_boundaries_t*,
				typename tlist_t::prefix_end_t
			> prefix_info_t;

	typedef unordered_set<
				prefix_info_t,
				prefix_hasher<prefix_info_t>,
				prefixes_test_equal<prefix_info_t>
			> prefix_set_t;

	static bool insertOrMerge(
			prefix_set_t& known_prefixes_info,
			tid_t transId,
			int32_t weight,
			typename tlist_t::transaction_boundaries_t* trans_boundaries,
			childItemT *end_prefix,
			IndexedTransactionsList<childItemT>* writer)
	{
		auto it = known_prefixes_info.emplace(
				transId, trans_boundaries, end_prefix);

		if (!it.second)
		{
			// the insertion was actually not done because
			// the element *it.first has the same prefix
			const prefix_info_t &found_match = *(it.first);

			// we should merge these 2 transactions
			TRANS_END(found_match) = std::set_intersection(
									PREFIX_END(found_match), TRANS_END(found_match),
									end_prefix, trans_boundaries->end_transaction,
									PREFIX_END(found_match));

			// set the support of *it to the sum
			// of the supports of these 2 transactions
			writer->incTransSupport(TID(found_match), weight);

			// discard the one we just wrote
			writer->discardLastTransaction();
		}

		return it.second;
	}
};

}
}
