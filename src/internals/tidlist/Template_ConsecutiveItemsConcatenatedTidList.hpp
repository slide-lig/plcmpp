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

#include <internals/tidlist/ConsecutiveItemsConcatenatedTidList.hpp>
#include <util/RawArray.hpp>

using util::RawArray;

namespace internals {
namespace tidlist {

template <class T>
class Template_ConsecutiveItemsConcatenatedTidList
    : public ConsecutiveItemsConcatenatedTidList
{

private:
	static const T MAX_VALUE;
    RawArray<T>* _array;
    T* _array_fast;

public:
	typedef T base_type;
    Template_ConsecutiveItemsConcatenatedTidList(
    		Counters* c, int32_t highestItem);
    Template_ConsecutiveItemsConcatenatedTidList(
    		const Template_ConsecutiveItemsConcatenatedTidList<T>& other);

    ~Template_ConsecutiveItemsConcatenatedTidList();


    unique_ptr<TidList::ItemTidList> getItemTidList(int32_t item) override;
    bool checkTidListsInclusion(int32_t item_a, int32_t item_b);

    unique_ptr<TidList> clone() override;

protected:
    void write(int32_t position, int32_t transaction) override;

public:
    static bool compatible(int32_t maxTid);
};

template <class T>
class Template_ItemTidList : public TidList::ItemTidList
{
public:
	T* begin;
	T* end;

public:
	Template_ItemTidList(T* begin, T* end);
	virtual ~Template_ItemTidList() {};
	unique_ptr<Iterator<int32_t> > iterator() override;
};

template <class T>
class Template_TidIterator : public Iterator<int32_t>
{
public:
	T* pos;
	T* end;

public:
    bool hasNext() override;
    int32_t next() override;

    Template_TidIterator(T* begin, T* end);
};

}
}

/* Template class, we need its definition */
#include <internals/tidlist/Template_ConsecutiveItemsConcatenatedTidList_impl.hpp>

