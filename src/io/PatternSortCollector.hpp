

#pragma once

#include <io/PatternsCollector.hpp>

namespace io {

class PatternSortCollector : public PatternsCollector
{

protected:
    PatternsCollector* decorated;

public:
    PatternSortCollector(PatternsCollector* wrapped);

    void collect(int32_t support, vector<int32_t>* pattern) override;
    int64_t close() override;
    int32_t getAveragePatternLength() override;
};

}
