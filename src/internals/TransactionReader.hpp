#pragma once

#include <cstdint>
using namespace std;

namespace internals {

class TransactionReader
{
public:
	virtual ~TransactionReader() {};
    virtual int32_t getTransactionSupport() = 0;
    virtual int32_t next() = 0;
    virtual bool hasNext() = 0;
};

}
