

#pragma once

#include <io/PatternsCollector.hpp>

namespace io {

class StdOutCollector
    : public virtual PatternsCollector
{

protected:
    int64_t collected;
    int64_t collectedLength;

public:
    StdOutCollector();
    void collect(int32_t support, vector<int32_t>* pattern) override;
    int64_t close() override;
    int32_t getAveragePatternLength() override;

};

}
