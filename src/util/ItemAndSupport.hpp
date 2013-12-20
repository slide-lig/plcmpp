
#pragma once

#include <cstdint>
#include <tuple>

using namespace std;

namespace util {

class ItemAndSupport : public tuple<uint32_t, uint32_t>
{
public:
	inline ItemAndSupport(uint32_t in_item, uint32_t in_support) :
	tuple<uint32_t, uint32_t>(in_support, in_item)
	{
	}

	inline uint32_t item() const {
		return get<1>(*this);
	}

	inline uint32_t support() const {
		return get<0>(*this);
	}
};

/* We sort according to the support (only). */
inline bool operator< (const ItemAndSupport &i1, const ItemAndSupport &i2)
{
    return i1.support() < i2.support();
}


}
