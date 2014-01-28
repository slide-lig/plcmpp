#pragma once

#include <cstdint>
using namespace std;

namespace internals {

template <class T>
class Template_TransactionReader
{
public:
	virtual ~Template_TransactionReader() {};
    virtual int32_t getTransactionSupport() = 0;
    virtual T next() = 0;
    virtual bool hasNext() = 0;
};

typedef Template_TransactionReader<int32_t> TransactionReader;

}
