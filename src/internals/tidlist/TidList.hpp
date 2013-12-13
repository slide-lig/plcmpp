
#pragma once

#include <cstdint>
#include <memory>
using namespace std;

#include "util/Iterator.hpp"
using namespace util;

namespace internals {
namespace tidlist {

class TidList {

public:
	struct TIntIterable
	{
		~TIntIterable();
	    virtual unique_ptr<Iterator<int32_t> > iterator() = 0;
	};

	TidList();
	virtual ~TidList();

	virtual unique_ptr<Iterator<int32_t> > get(int32_t item) = 0;
	virtual unique_ptr<TidList::TIntIterable>
						getIterable(int32_t item) = 0;
	virtual void addTransaction(int32_t item, int32_t transaction) = 0;
	virtual unique_ptr<TidList> clone() = 0;
};
}
}

