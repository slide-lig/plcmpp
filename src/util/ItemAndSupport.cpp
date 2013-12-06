#include <util/ItemAndSupport.hpp>

namespace util {

	uint32_t ItemAndSupport::item() {
		return get<0>(*this);
	}

	uint32_t ItemAndSupport::support() {
		return get<1>(*this);
	}
}

