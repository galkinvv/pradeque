#pragma once
#include <stdint.h>
typedef struct
{
    void* value;
    uintptr_t entry_packed_with_align_at_least;//pointer to table entry with extra data set to lower estimation of additional alignment bits to alignment of smallest block that were used for this block start and finish addresses;
    //for first arrays allocated while table address were unknown the entry pointer maybe zero - it would be loaded from known table offset
    //note that align_at_least is not required to be set to non-zero value even if such alignment exist
}
pradeque_iterator_t;

typedef struct
{
   void *first;
   void *post_last;
   uintptr_t last_entry_packed_with_hi_first_entry_index;//pointer to table entry of post_last with extra data set to high bits of table index for first entry
   //table entry can be zero until table is allocated; first_entry_index can be used to store info about relative positions of array parts

   uintptr_t lo_first_entry_index_packed_with_size;//high bits of table index for first entry with extra data set to total element count
}
pradeque_t;

typedef struct
{
    size_t value_size;//size of values stored in the container
    void* context;//context passed to callbacks
    
    //allocator
    void* (*aligned_alloc)(size_t alignment, size_t size, void* context);
    void (*aligned_free)(void *block, size_t alignment, size_t size, void* context);

    void (*releaser)(void* value, void* context);//element releaser
}
const pradeque_params_t;

pradeque_iterator_t pradeque_begin(const pradeque_t* deque, pradeque_params_t* params);
pradeque_iterator_t pradeque_end(const pradeque_t* deque, pradeque_params_t* params);

void pradeque_advance(pradeque_iterator_t* iterator, ptrdiff_t advance_by, pradeque_params_t* params);

void* pradeque_push_front(pradeque_t* deque, pradeque_params_t* params);
void* pradeque_push_back(pradeque_t* deque, pradeque_params_t* params);

void pradeque_pop_front(pradeque_t* deque, pradeque_params_t* params);
void pradeque_pop_back(pradeque_t* deque, pradeque_params_t* params);

void pradeque_release(pradeque_t* deque, pradeque_params_t* params);//releases all objects and allocated memory
