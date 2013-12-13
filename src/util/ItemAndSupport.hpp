
#pragma once

#include <cstdint>
#include <tuple>

using namespace std;

namespace util {

/* CAUTION: the support must be first in the tuple
 * because it is used for ordering them. */

class ItemAndSupport : public tuple<uint32_t, uint32_t>
{
public:
	inline ItemAndSupport(uint32_t in_item, uint32_t in_support) :
	tuple<uint32_t, uint32_t>(in_support, in_item)
	{
	}

	inline uint32_t item() {
		return get<1>(*this);
	}

	inline uint32_t support() {
		return get<0>(*this);
	}
};
}
