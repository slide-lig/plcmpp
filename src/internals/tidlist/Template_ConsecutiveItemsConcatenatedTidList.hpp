

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

public:
    Template_ConsecutiveItemsConcatenatedTidList(
    		Counters* c, int32_t highestItem);
    Template_ConsecutiveItemsConcatenatedTidList(
    		const Template_ConsecutiveItemsConcatenatedTidList<T>& other);

    ~Template_ConsecutiveItemsConcatenatedTidList();


    unique_ptr<TidList> clone() override;
    int32_t read(int32_t position) override;

protected:
    void write(int32_t position, int32_t transaction) override;

public:
    static bool compatible(int32_t maxTid);
};

}
}

/* Template class, we need its definition */
#include <internals/tidlist/Template_ConsecutiveItemsConcatenatedTidList_impl.hpp>

