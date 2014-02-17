
#pragma once

#include <cstdint>
#include <memory>
using namespace std;

#include "util/Iterator.hpp"
using namespace util;

namespace internals {

class Counters;

namespace tidlist {

class TidList {

protected:
	TidList();

public:
	struct ItemTidList
	{
		~ItemTidList();
	    virtual unique_ptr<Iterator<int32_t> > iterator() = 0;
	};

	virtual ~TidList();
	virtual unique_ptr<TidList::ItemTidList>
						getItemTidList(int32_t item) = 0;
	virtual void addTransaction(int32_t item, int32_t transaction) = 0;
	virtual unique_ptr<TidList> clone() = 0;
};
}
}

