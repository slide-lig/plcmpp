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
#include <tuple>

using namespace std;

namespace util {

class ItemAndSupport : public tuple<uint32_t, uint32_t>
{
public:
	inline ItemAndSupport(uint32_t in_item, uint32_t in_support) :
	tuple<uint32_t, uint32_t>(in_support, in_item)
	{
	}

	inline uint32_t item() const {
		return get<1>(*this);
	}

	inline uint32_t support() const {
		return get<0>(*this);
	}
};

/* We sort according to the support (only). */
inline bool operator< (const ItemAndSupport &i1, const ItemAndSupport &i2)
{
    return i1.support() < i2.support();
}


}
