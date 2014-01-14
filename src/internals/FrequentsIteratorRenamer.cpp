
#include <internals/FrequentsIteratorRenamer.hpp>

internals::FrequentsIteratorRenamer::FrequentsIteratorRenamer(
		FrequentsIterator* decorated, p_array_int32 itemsRenaming) {
	_renaming = itemsRenaming;
	_wrapped = decorated;
}

int32_t internals::FrequentsIteratorRenamer::next() {
	int32_t next = _wrapped->next();
	if (next >= 0) {
		return (*_renaming)[next];
	} else {
		return -1;
	}
}

int32_t internals::FrequentsIteratorRenamer::peek() {
	int32_t next = _wrapped->peek();
	if (next >= 0) {
		return (*_renaming)[next];
	} else {
		return -1;
	}
}

int32_t internals::FrequentsIteratorRenamer::last() {
	return (*_renaming)[_wrapped->last()];
}
