

#pragma once

#include <internals/tidlist/TidList.hpp>
#include <internals/Counters.hpp>
#include <util/shortcuts.h>

using util::p_array_int32;

namespace internals {
namespace tidlist {

class ConsecutiveItemsConcatenatedTidList
    : public TidList
{

protected:
    p_array_int32 _indexAndFreqs;
	int32_t* _indexAndFreqs_fast;
	uint32_t _indexAndFreqs_size;
    uint32_t _storage_size;

public:
    ConsecutiveItemsConcatenatedTidList(
    		Counters* c, int32_t highestTidList);
    ConsecutiveItemsConcatenatedTidList(
    		p_array_int32 lengths, int32_t highestTidList);
    ConsecutiveItemsConcatenatedTidList(
    		const ConsecutiveItemsConcatenatedTidList& other);

    ~ConsecutiveItemsConcatenatedTidList();

    void resetTidLists() override;
    void addTransaction(int32_t item, int32_t transaction) override;

protected:
    virtual void write(int32_t position, int32_t transaction) = 0;
};

}
}
