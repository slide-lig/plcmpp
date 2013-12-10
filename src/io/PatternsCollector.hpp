
#pragma once

#include <cstdint>
#include <vector>
using namespace std;

namespace io {

class PatternsCollector
{
public:
	virtual ~PatternsCollector() {};
    virtual void collect(int32_t support, vector<int32_t>* pattern) = 0;
    virtual int64_t close() = 0;
    virtual int32_t getAveragePatternLength() = 0;
};

}
