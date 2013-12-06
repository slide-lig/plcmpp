
#pragma once

#include <cstdint>
#include <tuple>

using namespace std;

namespace util {

class ItemAndSupport : public tuple<uint32_t, uint32_t>
{
public:
	inline uint32_t item();
	inline uint32_t support();
};
}
