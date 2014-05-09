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

#include <internals/tidlist/TidList.hpp>
#include <internals/Counters.hpp>
#include <util/shortcuts.h>

using util::p_array_int32;

namespace internals {
namespace tidlist {

class ConsecutiveItemsConcatenatedTidList
    : public TidList
{

protected:
    p_array_int32 _indexAndFreqs;
	int32_t* _indexAndFreqs_fast;
	uint32_t _indexAndFreqs_size;
    uint32_t _storage_size;

public:
    ConsecutiveItemsConcatenatedTidList(
    		Counters* c, int32_t highestTidList);
    ConsecutiveItemsConcatenatedTidList(
    		p_array_int32 lengths, int32_t highestTidList);
    ConsecutiveItemsConcatenatedTidList(
    		const ConsecutiveItemsConcatenatedTidList& other);

    ~ConsecutiveItemsConcatenatedTidList();

    void resetTidLists() override;
    void addTransaction(int32_t item, int32_t transaction) override;

protected:
    virtual void write(int32_t position, int32_t transaction) = 0;
};

}
}
