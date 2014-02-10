#pragma once

#include <cstring>
#include <vector>
using namespace std;

#include <assert.h>

namespace util {

template<class T>
class BlocksStorage {

private:
	enum STATUS_NEXT {
		NO_MORE_BLOCKS,
		NEXT_BLOCK_FOLLOWS,
		NEXT_BLOCK_ON_NEXT_CHUNK
	};

	static const uint32_t CHUNK_SIZE = 1024 * 1024;
	static const uint32_t LEVEL0_INIT_SIZE = 1024;
	vector<T*>* level0_vector;
	T* block_start;
	T* last_chunk_curr;
	T* prev_chunk_end;
	T* last_chunk_end;
	T* status_next;

	void add_chunk() {
		prev_chunk_end = last_chunk_end;
		last_chunk_curr = new T[CHUNK_SIZE];
		last_chunk_end = last_chunk_curr + CHUNK_SIZE;
		level0_vector->push_back(last_chunk_curr);
	}

public:
	class iterator {
	private:
		T* status_next;
		T** level0_ptr;


	public:
		iterator() {
			status_next = nullptr;
			level0_ptr = nullptr;
		}
		iterator(T** in_level0_data) : level0_ptr(in_level0_data) {
			status_next = level0_ptr[0];
		}

		bool has_more_blocks() {
			return *status_next != STATUS_NEXT::NO_MORE_BLOCKS;
		}

		void next_block(T*& ref_block_start, T*& ref_block_end)
		{
			if (*status_next == STATUS_NEXT::NEXT_BLOCK_FOLLOWS)
			{
				ref_block_start = status_next+2;
				ref_block_end = ref_block_start + (*(status_next+1));
			}
			else
			{	// *status_next == NEXT_BLOCK_ON_NEXT_CHUNK
				++level0_ptr;
				ref_block_start = *level0_ptr+1;
				ref_block_end = ref_block_start + (*(*level0_ptr));
			}

			// update status_next
			if (ref_block_end == *level0_ptr + CHUNK_SIZE)
			{
				++level0_ptr;
				status_next = *level0_ptr;
			}
			else
			{
				status_next = ref_block_end;
			}
		}
	};

	BlocksStorage() {
		level0_vector = new vector<T*>();
		level0_vector->reserve(LEVEL0_INIT_SIZE);
		add_chunk();
		status_next = push_back(STATUS_NEXT::NO_MORE_BLOCKS);
		block_start = nullptr;
		prev_chunk_end = nullptr;
	}

	~BlocksStorage() {
		for (auto it = level0_vector->begin(); it < level0_vector->end();
				++it) {
			delete[] (*it);
		}
		delete level0_vector;
	}

	void start_block() {
		if (last_chunk_curr == last_chunk_end) {
			add_chunk();
			*status_next = STATUS_NEXT::NEXT_BLOCK_ON_NEXT_CHUNK;
		} else {
			*status_next = STATUS_NEXT::NEXT_BLOCK_FOLLOWS;
		}
		block_start = last_chunk_curr;
		// first integer is the size of the block
		// we reserve it
		push_back(0);
	}

	T* push_back(T elem) {
		if (last_chunk_curr == last_chunk_end) {
			add_chunk();
			if (block_start != nullptr) {// avoid the block to span across the chunk boundary
										 // -> shift it to the start of the new chunk
				std::copy(block_start, prev_chunk_end, last_chunk_curr);
				T* new_block_start = last_chunk_curr;
				last_chunk_curr += prev_chunk_end - block_start;
				block_start = new_block_start;
				*status_next = STATUS_NEXT::NEXT_BLOCK_ON_NEXT_CHUNK;
			}
		}
		*last_chunk_curr = elem;
		return last_chunk_curr++;
	}

	void end_block(T* &items_begin, T* &items_end) {
		items_begin = block_start + 1;
		items_end = last_chunk_curr;
		end_block();
	}

	void end_block() {
		auto len = last_chunk_curr - block_start - 1;
		*block_start = len;
		block_start = nullptr;
		status_next = push_back(STATUS_NEXT::NO_MORE_BLOCKS);
		// status_next will be overwritten if there is a later
		// call to start_block().
	}

	iterator getIterator() {
		return iterator(level0_vector->data());
	}


};

} /* namespace util */

