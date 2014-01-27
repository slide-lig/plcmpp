

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

private:
    p_array_int32 _indexAndFreqs;
	int32_t* _indexAndFreqs_fast;
	uint32_t _indexAndFreqs_size;
protected:
    uint32_t _storage_size;

public:
    ConsecutiveItemsConcatenatedTidList(
    		Counters* c, int32_t highestTidList);
    ConsecutiveItemsConcatenatedTidList(
    		p_array_int32 lengths, int32_t highestTidList);
    ConsecutiveItemsConcatenatedTidList(
    		const ConsecutiveItemsConcatenatedTidList& other);

    ~ConsecutiveItemsConcatenatedTidList();

    unique_ptr<Iterator<int32_t> > get(int32_t item) override;
    unique_ptr<TidList::TIntIterable> getIterable(int32_t item) override;
    void addTransaction(int32_t item, int32_t transaction) override;
    virtual int32_t read(int32_t position) = 0;

protected:
    virtual void write(int32_t position, int32_t transaction) = 0;
};

class TidIterable : public TidList::TIntIterable
{
private:
	ConsecutiveItemsConcatenatedTidList *_tidlist;
	int32_t _item;

public:
	TidIterable(
			ConsecutiveItemsConcatenatedTidList *tidlist,
			int32_t item);
	unique_ptr<Iterator<int32_t> > iterator() override;
};

class TidIterator : public Iterator<int32_t>
{
private:
	ConsecutiveItemsConcatenatedTidList *_tidlist;
    int32_t _index;
    int32_t _length;
    int32_t _startPos;

public:
    bool hasNext() override;
    int32_t next() override;

    TidIterator(
    		ConsecutiveItemsConcatenatedTidList *tidlist,
    		int32_t length, int32_t startPos);
};


}
}
