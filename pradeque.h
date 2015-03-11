#pragma once
#include <stddef.h>
#include <stdint.h>
typedef struct
{
    void* value;//points to value stored in container. Can be NULL only for past-the-end iterator pointing to yet-not-allocated table
	uintptr_t table_entry_id;
	//id is implemened as  packed value with pointer to table and extra flags used for alignment (if non-zero pointer) or indicating of techical data placement(if zero pointer).
	//pointer to table entry with extra data set to lower estimation of additional alignment bits to alignment of smallest block that were used for this block start and finish addresses;
	//for first arrays allocated while table address were unknown the entry pointer maybe zero - it would be loaded from known table offset
	//note that align_at_least is not required to be set to non-zero value even if such alignment exist
}
pradeque_iterator_t;

typedef struct
{
    void* first; //points to first element of block
	void *post_last;//points after last element of block
	//Al fields are NULL/0 for past-the-end block iterator. Pre-begin  iterator equals past the end.
	//non-NULL first and post_last are never equal.
	uintptr_t table_entry_id; //see pradeque_iterator_t definition
}
pradeque_contigous_block_iterator_t;

typedef struct
{
   struct {
	   void *first;
	   void *post_last;
	   uintptr_t last_entry_packed_with_lo_first_entry_index;//pointer to table entry of post_last with extra data set to low bits of table index for first entry
	   //table entry can be zero until table is allocated; first_entry_index can be used to store info about relative positions of array parts

	   uintptr_t hi_first_entry_index_packed_with_size;//high bits of table index for first entry with extra data set to total element count
   } detail; //pradeque_t details are not part of API
}
pradeque_t;

//TODO: pass only needed parameters to every function
typedef struct pradeque_params_nonconst_struct_t
{
    size_t value_size;//size of values stored in the container. All values are aligned to maximal power of two that divides size
    //instances are always aligned to the greatest power of 2 dividing value_size
	//memory allocator
    void* (*aligned_alloc)(size_t alignment, size_t size, const struct pradeque_params_nonconst_struct_t* context);//aligned memory allocate function or NULL to use aligned_alloc
    void (*aligned_free)(void *block, size_t alignment, size_t size, const struct pradeque_params_nonconst_struct_t* context);//aligned memory free function or NULL to use free
}
const pradeque_params_t;

pradeque_iterator_t pradeque_begin(const pradeque_t* deque, pradeque_params_t* params);
pradeque_iterator_t pradeque_end(const pradeque_t* deque, pradeque_params_t* params);

void pradeque_advance(pradeque_iterator_t* iterator_to_advance, intptr_t advance_by, pradeque_params_t* params);

//returns iterator being start of added region
pradeque_iterator_t pradeque_push_front(pradeque_t* deque, intptr_t count_to_add, pradeque_params_t* params);
//returns iterator being end of added region
pradeque_iterator_t pradeque_push_back(pradeque_t* deque, intptr_t count_to_add, pradeque_params_t* params);

//returns count of erased elements
intptr_t pradeque_pop_front(pradeque_t* deque, pradeque_iterator_t pop_to, pradeque_params_t* params);
intptr_t pradeque_pop_back(pradeque_t* deque, pradeque_iterator_t pop_from, pradeque_params_t* params);

intptr_t pradeque_distance(const pradeque_iterator_t* first, const pradeque_iterator_t* last, pradeque_params_t* params); //can be implemented without iterating all contigous blocks
pradeque_contigous_block_iterator_t pradeque_next_contigous_block(pradeque_contigous_block_iterator_t previous_or_start, const pradeque_iterator_t* first, const pradeque_iterator_t* last, pradeque_params_t* params);
pradeque_contigous_block_iterator_t pradeque_prev_contigous_block(pradeque_contigous_block_iterator_t next_or_end, const pradeque_iterator_t* first, const pradeque_iterator_t* last, pradeque_params_t* params);

void pradeque_clear(pradeque_t* deque, pradeque_params_t* params);//releases allocated memory

//sizes are expressed in signed types because pradeque does not allow such big sizes that does fit in unsigned types but doesn't fit in unsigned. This is expressed in API as result types
intptr_t pradeque_size(const pradeque_t* deque, pradeque_params_t* params);
intptr_t pradeque_max_size(size_t value_size)
