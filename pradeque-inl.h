#pragma once
#include "pradeque.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>

#define PRA_DEQUE_DETAIL_EMPTY
#define PRA_DEQUE_DETAIL_DEFER(id) id PRA_DEQUE_DETAIL_EMPTY
#define PRA_DEQUE_DETAIL_EXPAND(id) id

#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER_WITH_SHIFT(x, half_bits, shift_width) (shift_width + (PRA_DEQUE_DETAIL_DEFER(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER##half_bits)((x >> shift_width))))

#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER(x, half_bits) PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER_WITH_SHIFT(x, half_bits, ((x >> half_bits) ? half_bits : 0))
#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER2(x)  x>>1
#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER4(x)  PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER(x,2)
#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER8(x)  PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER(x,4)
#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER16(x) PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER(x,8)
#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER32(x) PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER(x,16)
#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER64(x) PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER(x,32)
#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(x) (PRA_DEQUE_DETAIL_EXPAND(PRA_DEQUE_DETAIL_EXPAND(PRA_DEQUE_DETAIL_EXPAND(PRA_DEQUE_DETAIL_EXPAND(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER32(((uint32_t)(x))))))))
#define PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(x) (PRA_DEQUE_DETAIL_EXPAND(PRA_DEQUE_DETAIL_EXPAND(PRA_DEQUE_DETAIL_EXPAND(PRA_DEQUE_DETAIL_EXPAND(PRA_DEQUE_DETAIL_EXPAND(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT_HELPER64(((uint64_t)(x)))))))))

enum{

kPradequeDetailPointerAlign = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(sizeof(void*)),
kPradequeDetailAddressBits = sizeof(void*) * CHAR_BIT,

//count of bits required to adress element in a table of block entries
//the value should be smaller to minimize memory overhead for table allocation, which is done with quite small number of elements
//the value should be greatear to allow more effective buffer usage by allowing more buffer sizes
//the selected tecnbhique is to use 3 buffers of every size with sizes exponentially growing by 4
//because very big and very smal containers are not used 32 table entryes are enough to store one exponential sequence of blocks. It must be doubled for allowing both back and front exponential growth.
//so 6 bit is enough for 32 bit case.
//for doubling/halfing adddress space with the formula adds/subtracts a bit. This give nice result for current 64-bit systems (allowing up to 2^48 sizes) and meaningful result for 16 and 128 bit systems, so it looks good.

kPradequeDetailFullTableIndexBits = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(kPradequeDetailAddressBits) + 1,

kPradequeDetailHalfTableSize = 1 << (kPradequeDetailFullTableIndexBits - 1),
kPradequeDetailHalfTableSmallSpecialEntries = 1, //count of first element that does not correspond to standard size calculating formula.
kPradequeDetailHalfTableBigDupEntries = 4, //count of end entries with identical size.
//bits in table pointer containing mode info
kPradequeDetailBitsInTablePointerForModeDirection = 1, //backward or forward
kPradequeDetailBitsInTablePointerForModeShift = 2 + PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(kPradequeDetailHalfTableBigDupEntries - 1), //on both table sides there are 4*2 = 8 max blocks, they can be shifted by any value.
kPradequeDetailBitsInTablePointerForLastBigBlock = kPradequeDetailBitsInTablePointerForModeShift, //to be able calculate whether big blocks belong to pre-zero or to post-zero part.
//bits in every table pointer
kPradequeDetailBitsInEveryTablePointer = 2, //four possible values: not allocated, allocated but unused, used with plain address mode, used with non-plain address mode
kPradequeDetailBitsInSpecialTablePointer = kPradequeDetailBitsInEveryTablePointer + kPradequeDetailBitsInTablePointerForModeDirection + kPradequeDetailBitsInTablePointerForModeShift + kPradequeDetailBitsInTablePointerForLastBigBlock,
kPradequeDetailSpecialTablePointerPosition = 8, //shares the same cache line with table start and is expected to store quite big block
kPradequeDetailBlockAlignForNoTableMode = 1 + kPradequeDetailPointerAlign, //to store pointer pointer we need at least pointer size. Half-block is allocated for system information, so the block must have one more align.
kPradequeDetailMinBlockAlignToFitSpecial = kPradequeDetailBitsInSpecialTablePointer - 2 - ((kPradequeDetailSpecialTablePointerPosition - 2) / 3) * 2,//number of entirely fitted 3-same-block table entries.
kPradequeDetailMinBlockAlignToFitTableBits = kPradequeDetailMinBlockAlignToFitSpecial > kPradequeDetailBitsInEveryTablePointer ? kPradequeDetailMinBlockAlignToFitSpecial : kPradequeDetailBitsInEveryTablePointer, 
kPradequeDetailMinBlockAlign = kPradequeDetailBlockAlignForNoTableMode > kPradequeDetailMinBlockAlignToFitTableBits ? kPradequeDetailBlockAlignForNoTableMode : kPradequeDetailMinBlockAlignToFitTableBits, 
kPradequeDetailFirstAllocationMinOptimalBytes = 64, //byte count being optimal for first allocation. Taken as one cache line. gcc std deque uses 512.

//table size defines the relation between smallest and biggesst arrays.
kPradequeDetailHalfTableGroupedBy3Entries = (kPradequeDetailHalfTableSize - kPradequeDetailHalfTableSmallSpecialEntries - kPradequeDetailHalfTableBigDupEntries) / 3,
kPradequeDetailLog2DiffBetweenSmallAndBigBlocks = 2 * (kPradequeDetailHalfTableGroupedBy3Entries - 1) + 2 + 2,//first +2 is for small->standard, second +2 is for standard->big
};

//utility functions

static inline int praDequeDetail_LowerLog2(uintptr_t i)
{
   return PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(i);
}

static inline int praDequeDetail_UpperLog2(uintptr_t i)
{
   return (i <= 1) ? 0 : 1 + praDequeDetail_LowerLog2(i - 1);
}

static inline uintptr_t praDequeDetail_PackedExtraMask(int extra_bits)
{
    assert(extra_bits > 0);
    return ((uintptr_t)1 << extra_bits) - 1;
}

static inline uintptr_t praDequeDetail_PackedGetMainBeingMultiple(uintptr_t packed, int extra_bits)
{
    return (packed & ~praDequeDetail_PackedExtraMask(extra_bits));
}

static inline uintptr_t praDequeDetail_PackedSetMainBeingMultiple(uintptr_t main_being_multiple, uintptr_t packed, int extra_bits)
{
    assert(!(main_being_multiple & praDequeDetail_PackedExtraMask(extra_bits)));
    return main_being_multiple | (praDequeDetail_PackedExtraMask(extra_bits) & packed);//keep extra but change main
}

//domain functions
//return bit limitation of maximal size calculating maximal how much objects of size rounded up to next power of two can be placed in half of address space
static inline int praDequeDetail_MaxSizeBitsUpToDeg2InHalfAddressSpace(size_t value_size)
{
    return kPradequeDetailAddressBits - 1 - praDequeDetail_UpperLog2(value_size);
}

//Calculates log2 of entries count in small block
static inline int praDequeDetail_SmallBlockEntriesLog2(size_t value_size, size_t size_aligned_bits)
{
    const int min_entries_to_fill_optimal_bytes = kPradequeDetailFirstAllocationMinOptimalBytes / (value_size * 2); //mul by 2 corresponds to initialy allocating 2 blocks
	const int min_entries_to_fill_optimal_bytes_log2 = praDequeDetail_LowerLog2(min_entries_to_fill_optimal_bytes);
	const int min_log2_to_achieve_align = kPradequeDetailMinBlockAlign - size_aligned_bits;
    return min_entries_to_fill_optimal_bytes_log2 > min_log2_to_achieve_align ? min_entries_to_fill_optimal_bytes_log2 : min_log2_to_achieve_align;
}

//API implementation
#ifndef PRA_DEQUE_DETAIL_API
#define PRA_DEQUE_DETAIL_API inline
#endif

PRA_DEQUE_DETAIL_API
void pradeque_clear(pradeque_t* deque, pradeque_params_t* params, pradeque_deallocator_t deallocator){/*TODO*/}

PRA_DEQUE_DETAIL_API
pradeque_params_t pradeque_prepare_params(size_t value_size)
{
	assert(value_size);
	int size_aligned_bits = 0;
	while(0 == (value_size & ( 1 << size_aligned_bits)))
	{
		++size_aligned_bits;
	}
	const size_t odd_value_size = value_size / ( 1 << size_aligned_bits);
	const int small_block_entries_log2 = praDequeDetail_SmallBlockEntriesLog2(value_size, size_aligned_bits);

	//TODO - enable calculation
	intptr_t odd_mul_to_get_1_mod_max_block = 0; //Bezout coefficient for odd_value_size and max block size. Needed for calculation the block range from any inner block pointer.

    const int max_bits_heap = praDequeDetail_MaxSizeBitsUpToDeg2InHalfAddressSpace(value_size);//maximal allocatable array is limited by address space
    const int max_bits_table = small_block_entries_log2 + kPradequeDetailLog2DiffBetweenSmallAndBigBlocks + praDequeDetail_LowerLog2(kPradequeDetailHalfTableBigDupEntries) + 1;//log2 of element count that can be placed in big blocks in table. +1 corresponds to table consisting of two halves.
	
	const int max_bits = max_bits_heap < max_bits_table ? max_bits_heap : max_bits_table;
	const intptr_t max_size = ((intptr_t)1) << max_bits;
	assert(max_size > 0);//to be sure that any difference fill in intptr_t
	pradeque_params_t params = {value_size, max_size, {size_aligned_bits, small_block_entries_log2, odd_value_size, odd_mul_to_get_1_mod_max_block}};
	return params;
}

PRA_DEQUE_DETAIL_API
void* pradeque_allocator_default(size_t alignment, size_t size, int zero_on_allocation, const pradeque_t* context)
{
	assert(size);
	assert(alignment);
	assert(0 == size % alignment);
	assert(!zero_on_allocation || size == (kPradequeDetailHalfTableSize * 2 * sizeof(void*)));
	assert(context);
	void* result = aligned_alloc(alignment, size); //TODO: implement workaround for systems without aligned_alloc
	if (zero_on_allocation)
	{
		//TODO: tell compiler that result is aligned at least at min_block/2 or table size
		memset(result, 0, size);
	}
	return result;
}

PRA_DEQUE_DETAIL_API
void pradeque_deallocator_default(void *block, size_t alignment, size_t size, int zero_on_allocation, const pradeque_t* context)
{
	assert(block);
	assert(size);
	assert(alignment);
	assert(0 == size % alignment);
	assert(!zero_on_allocation || size == (kPradequeDetailHalfTableSize * 2 * sizeof(void*)));
	assert(context);
	free(block);
}

