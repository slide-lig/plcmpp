

#pragma once

#include <vector>
using namespace std;

#include <internals/FrequentsIterator.hpp>

namespace internals {

class FrequentsIteratorRenamer : public virtual FrequentsIterator
{
private:
    vector<int32_t>* _renaming;
    FrequentsIterator* _wrapped;

public:
    FrequentsIteratorRenamer(
    		FrequentsIterator* decorated,
    		vector<int32_t>* itemsRenaming);

    int32_t next() override;
    int32_t peek() override;
    int32_t last() override;
};

}

