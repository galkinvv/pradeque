#pragma once
#include <stddef.h>
#include <stdint.h>
typedef struct
{
    void* value; //this field is part of API
    struct {
		uintptr_t entry_packed_with_align_at_least;//pointer to table entry with extra data set to lower estimation of additional alignment bits to alignment of smallest block that were used for this block start and finish addresses;
		//for first arrays allocated while table address were unknown the entry pointer maybe zero - it would be loaded from known table offset
		//note that align_at_least is not required to be set to non-zero value even if such alignment exist
	} detail;
}
pradeque_iterator_t;

typedef struct
{
   struct {
	   void *first;
	   void *post_last;
	   uintptr_t last_entry_packed_with_lo_first_entry_index;//pointer to table entry of post_last with extra data set to low bits of table index for first entry
	   //table entry can be zero until table is allocated; first_entry_index can be used to store info about relative positions of array parts

	   uintptr_t hi_first_entry_index_packed_with_size;//high bits of table index for first entry with extra data set to total element count
   } detail;
}
pradeque_t;

typedef struct pradeque_params_nonconst_struct_t
{
    size_t value_size;//size of values stored in the container
	size_t min_align;//minimal alignment or 0 if default alignment is sufficient

	//memory allocator
    void* (*aligned_alloc)(size_t alignment, size_t size, const struct pradeque_params_nonconst_struct_t* context);//aligned memory allocate function or NULL to use aligned_alloc
    void (*aligned_free)(void *block, size_t alignment, size_t size, const struct pradeque_params_nonconst_struct_t* context);//aligned memory free function or NULL to use free

    void (*releaser)(void* values_start, size_t values_count, const struct pradeque_params_nonconst_struct_t* context);//element releaser or NULL if not required
}
const pradeque_params_t;

pradeque_iterator_t pradeque_begin(const pradeque_t* deque, pradeque_params_t* params);
pradeque_iterator_t pradeque_end(const pradeque_t* deque, pradeque_params_t* params);

void pradeque_advance(pradeque_iterator_t* iterator_to_advance, intptr_t advance_by, pradeque_params_t* params);
intptr_t pradeque_distance(const pradeque_iterator_t* first, const pradeque_iterator_t* last, pradeque_params_t* params);

//push functions returns  pointers to newly allocated memory
void* pradeque_push_front(pradeque_t* deque, pradeque_params_t* params);
void* pradeque_push_back(pradeque_t* deque, pradeque_params_t* params);

void pradeque_pop_front(pradeque_t* deque, pradeque_params_t* params);
void pradeque_pop_back(pradeque_t* deque, pradeque_params_t* params);

void pradeque_release(pradeque_t* deque, pradeque_params_t* params);//releases all objects and allocated memory

//sizes are expressed in signed types because pradeque does not allow such big sizes that does fit in unsigned types but doesn't fit in unsigned. This is expressed in API as result types
intptr_t pradeque_size(const pradeque_t* deque, pradeque_params_t* params);
intptr_t pradeque_max_size(pradeque_params_t* params);
