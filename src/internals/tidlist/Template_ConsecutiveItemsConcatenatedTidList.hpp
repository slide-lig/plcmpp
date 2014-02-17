

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

