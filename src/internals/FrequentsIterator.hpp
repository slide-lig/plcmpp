

#pragma once

#include <cstdint>
using namespace std;

struct FrequentsIterator
{
	virtual ~FrequentsIterator();
	virtual int32_t next() = 0;
    virtual int32_t peek() = 0;
    virtual int32_t last() = 0;
};
