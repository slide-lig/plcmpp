

#pragma once

#include "util/shortcuts.h"
using namespace util;

#include <internals/FrequentsIterator.hpp>

namespace internals {

class FrequentsIteratorRenamer : public virtual FrequentsIterator
{
private:
    p_array_int32 _renaming;
    FrequentsIterator* _wrapped;

public:
    FrequentsIteratorRenamer(
    		FrequentsIterator* decorated,
    		p_array_int32 itemsRenaming);

    int32_t next() override;
    int32_t peek() override;
    int32_t last() override;
};

}

