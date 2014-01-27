

#pragma once

#include <atomic>
#include <io/PatternsCollector.hpp>


namespace io {

class NullCollector : public PatternsCollector
{

protected:
    std::atomic<int32_t> collectedCount;
    std::atomic<int64_t> collectedLength;

public:
    NullCollector();

    void collect(int32_t support, vector<int32_t>* pattern) override;
    int64_t close() override;
    int32_t getAveragePatternLength() override;
};

}
