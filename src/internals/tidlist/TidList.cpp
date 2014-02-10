
#include <climits>
using namespace std;

#include <internals/tidlist/TidList.hpp>
#include "internals/tidlist/Uint8ConsecutiveItemsConcatenatedTidList.hpp"
#include "internals/tidlist/Uint16ConsecutiveItemsConcatenatedTidList.hpp"
#include "internals/tidlist/Uint32ConsecutiveItemsConcatenatedTidList.hpp"

namespace internals {
namespace tidlist {

TidList::TidList() {
}

TidList::~TidList() {
}

TidList::ItemTidList::~ItemTidList() {
}

unique_ptr<TidList> TidList::newEmptyTidList(
		Counters* counters, int32_t max_tid) {
	TidList *tidlist;
	if (Uint8ConsecutiveItemsConcatenatedTidList::compatible(max_tid)) {
		tidlist = new Uint8ConsecutiveItemsConcatenatedTidList(
				counters, INT_MAX);
	} else if (Uint16ConsecutiveItemsConcatenatedTidList::compatible(max_tid)) {
		tidlist = new Uint16ConsecutiveItemsConcatenatedTidList(
				counters, INT_MAX);
	} else {
		tidlist = new Uint32ConsecutiveItemsConcatenatedTidList(
				counters, INT_MAX);
	}
	return unique_ptr<TidList>(tidlist);
}

}
}
