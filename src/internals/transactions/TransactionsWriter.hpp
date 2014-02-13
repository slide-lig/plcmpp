

#pragma once

namespace internals {
namespace transactions {

struct TransactionsWriter
{
	virtual ~TransactionsWriter() {};
    virtual int32_t beginTransaction(int32_t support) = 0;
    virtual void addItem(int32_t item) = 0;
    virtual void endTransaction(int32_t max_candidate) = 0;
};

}
}
