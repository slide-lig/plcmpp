/*******************************************************************************
 * Copyright (c) 2014 Etienne Dublé, Martin Kirchgessner, Vincent Leroy, Alexandre Termier, CNRS and Université Joseph Fourier.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the LICENSE.txt file joined with this program.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

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
	virtual void resetTidLists() = 0;
};
}
}

