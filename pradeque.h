#ifndef PRADEQUE_VERSION
#define PRADEQUE_VERSION 1
#include <stddef.h>
#include <stdint.h>
typedef struct pradeque_iterator_detailV1
{
    void* value;//points to value stored in container. Can be NULL only for past-the-end iterator pointing to yet-not-allocated table
	uintptr_t table_entry_id;
	//id is implemened as  packed value with pointer to table(zero means no-table mode), table logical index (used to track the order of blocks for no-table mode)
	//and extra flags used for indicating plain adressing mode usage (if with-table mode) or indicating of techical data placement(ino-table mode).
	//pointer to table entry with extra data set to lower estimation of additional alignment bits to alignment of smallest block that were used for this block start and finish addresses;
	//for first arrays allocated while table address were unknown the entry pointer maybe zero - it would be loaded from known table offset
	//note that align_at_least is not required to be set to non-zero value even if such alignment exist
}
pradeque_iterator_t;

typedef struct pradeque_contigous_block_iteration_detailV1
{
    pradeque_iterator_t first; //points to first element of contigous block
	void *post_last;//points after last element of contigous block
	intptr_t advance; //advance that is expected to be itearted.
}
pradeque_contigous_block_iteration_t; //it is not iterator with STL semantics. It imore correspond to iteration process that can be controled via this object

typedef struct
{
	pradeque_contigous_block_iteration_t iteration;
}pradeque_pop_iteration_t;//tiny wrapper to protect from misusing generic iteration_t object in pop operations

struct pradeque_detailV1;
typedef struct pradeque_detailV1 pradeque_t;

typedef struct
{
    const size_t value_size;//size of values stored in the container. All values are aligned to maximal power of two that divides size
	const intptr_t max_size;//sizes are expressed in signed types because pradeque does not allow such big sizes that does fit in unsigned types but doesn't fit in unsigned. This is expressed in API as types
	const struct
	{
		int size_aligned_bits;//size is demultiplied in power of two
		int small_block_entries_log2;//log2 of entries count in small block
		size_t odd_value_size;//and odd number
		intptr_t odd_mul_to_get_1_mod_max_block; //Bezout coefficient for odd_value_size and max block size. Needed for calculation the block range from any inner block pointer.
	}detailV1;
}
pradeque_params_t;


#define PRADEQUE_PREPARED_PARAMS(size_t__value_size) { PRADEQUE_PREPARED_PARAMS_DETAILV1(((size_t)(size_t__value_size)) /*generates comma-separated initializers*/}
//object instances are always aligned to the greatest power of 2 dividing value_size
//gets single parameter: value_size of type size_t
//this must be done in compile time.
//it is absolutely needed fo efficiency, and for constexpr max_size() in c++.
//The macro implementation is possible but is quite complicated.


typedef void* pradeque_allocator_t(size_t alignment, size_t size, int zero_on_allocation, const pradeque_t* context);//aligned memory allocate function
typedef void pradeque_deallocator_t(void *block, size_t alignment, size_t size, int zero_on_allocation, const pradeque_t* context);//memory deallocate function that is passed all extra parameters used for allocation

static void* pradeque_allocator_default(size_t alignment, size_t size, int zero_on_allocation, const pradeque_t* context);
static void pradeque_deallocator_default(void *block, size_t alignment, size_t size, int zero_on_allocation, const pradeque_t* context);

static pradeque_iterator_t pradeque_begin(const pradeque_t* deque, pradeque_params_t* params);
static pradeque_iterator_t pradeque_end(const pradeque_t* deque, pradeque_params_t* params);

static intptr_t pradeque_distance(pradeque_iterator_t first, pradeque_iterator_t last, pradeque_params_t* params); //can be implemented without iterating all contigous blocks
static pradeque_iterator_t pradeque_advance(pradeque_iterator_t iterator_to_advance, intptr_t advance_by, pradeque_params_t* params);//can be implemented without iterating all contigous blocks
//by there is no size method; however it's efficiency can be more that begin + end + distance if in indirect mode adressing params are loaded from deque not from table
//may be distance_fast method can be introduced to get faser distance with known deque object. May be not only distance

//returns pushed contigous block and remaining begin advance
static pradeque_contigous_block_iteration_t pradeque_push_front(pradeque_t* deque, intptr_t begin_advance/*always <=0*/, pradeque_params_t* params, pradeque_allocator_t allocator);
//returns pushed contigous block and remaining end advance
static pradeque_contigous_block_iteration_t pradeque_push_back(pradeque_t* deque, intptr_t end_advance/*always >=0*/, pradeque_params_t* params, pradeque_allocator_t allocator);

//removing requires knowtion of removed elements before theay are erased. So there is one more function called before first pop
static pradeque_pop_iteration_t pradeque_pop_front_prepare(const pradeque_t* deque, intptr_t begin_advance/*always >=0*/, pradeque_params_t* params);
static pradeque_pop_iteration_t pradeque_pop_front(pradeque_t* deque, pradeque_pop_iteration_t block_to_pop/*advane always >=0*/, pradeque_params_t* params, pradeque_deallocator_t deallocator);

static pradeque_pop_iteration_t pradeque_pop_back_prepare(const pradeque_t* deque, intptr_t end_advance/*always <=0*/, pradeque_params_t* params);
static pradeque_pop_iteration_t pradeque_pop_back(pradeque_t* deque, pradeque_pop_iteration_t block_to_pop/*advane always <=0*/, pradeque_params_t* params, pradeque_deallocator_t deallocator);

//to begin such iteartion a contigous block with zero size corresponding to some iteartor and expected advance must be prepared
static pradeque_contigous_block_iteration_t pradeque_next_contigous_block(pradeque_contigous_block_iteration_t current/*advance always >=0*/, pradeque_params_t* params);
static pradeque_contigous_block_iteration_t pradeque_prev_contigous_block(pradeque_contigous_block_iteration_t current/*advance always <=0*/, pradeque_params_t* params);

static void pradeque_clear(pradeque_t* deque, pradeque_params_t* params, pradeque_deallocator_t deallocator);//releases all allocated memory including caches (table, first block etc)

//inline implementation is included after all declaration to be sure that they are compilable without it
#include "pradeque-inl.h"
#else
#if PRADEQUE_VERSION != 1
#error pradeque.h with another version PRADEQUE_VERSION is already included. This is no suppored for single compilation module.
#endif
#endif
