#pragma once
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

enum PradequeDetailConstants{

kPradequeDetailUnusedPPVoidBits = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(sizeof(void*)),
kPradequeDetailAddressBits = sizeof(void*) * CHAR_BIT,

//count of bits required to adress element in a table of block entries
//the value should be smaller to minimize memory overhead for table allocation, which is done with quite small number of elements
//the value should be greatear to allow more effective buffer usage by allowing more buffer sizes
//the selected tecnbhique is to use 3 buffers of every size with sizes exponentially growing by 4
//because very big and very smal containers are not used 32 table eentryes are enough to store one exponential sequence of blocks. It must be doubled for allowing both back and front exponential growth.
//so 6 bit is enough for 32 bit case.
//for doubling/halfing adddress space with the formula adds/subtracts a bit. This give nice result for current 64-bit systems (allowing up to 2^48 sizes) and meaningful result for 16 and 128 bit systems, so it looks good.

kPradequeDetailTableIndexBits = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(kPradequeDetailAddressBits) + 1,

//limitation to size bits coming from storage scheme: pradeque_t stores in two intptr_t packed fields pointer to last table entry, first table entry index and size.
kPradequeDetailSizeBits = 2 * kPradequeDetailAddressBits - (kPradequeDetailAddressBits - kPradequeDetailUnusedPPVoidBits) - kPradequeDetailTableIndexBits,

kPradequeDetailHalfTableSize = 1 << (kPradequeDetailTableIndexBits - 1),
kPradequeDetailHalfTableSmallSpecialEntries = 2, //count of first element that does not correspond to standard size calculating formula.
kPradequeDetailHalfTableSmallSpecial0MinSize = sizeof(void*)*2, //min object count of special entry 0. In first version to simplicity is much enouhg to place pointer in half of block
kPradequeDetailHalfTableSmallSpecial1MinSize = 3*kPradequeDetailHalfTableSmallSpecial0MinSize, //min size of special entry 1
kPradequeDetailHalfTableSmallSpecialTotalMinBits = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(kPradequeDetailHalfTableSmallSpecial0MinSize + kPradequeDetailHalfTableSmallSpecial1MinSize), //min bitness of total elements in all small entries
kPradequeDetailFirstAllocationMaxOptimalBytes = 512, //byte count being optimal for first allocation (size for gcc std deque, also leads to minimal block size 32 for 64-bit objects)

//table size defines the relation between smallest and biggesst arrays.
kPradequeDetailHalfTableGroupedBy3Entries = (kPradequeDetailHalfTableSize - kPradequeDetailHalfTableSmallSpecialEntries) / 3,
kPradequeDetailLog2OfCapacityGrowByAddingAllGroupedBy3 = 2 * kPradequeDetailHalfTableGroupedBy3Entries
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

static inline uintptr_t praDequeDetail_PackedGetExtra(uintptr_t packed, int extra_bits)
{
    return packed & praDequeDetail_PackedExtraMask(extra_bits);
}

static inline uintptr_t praDequeDetail_Packed(uintptr_t main_being_multiple, uintptr_t extra, int extra_bits)
{
    assert(extra <= praDequeDetail_PackedExtraMask(extra_bits));
    assert(!(main_being_multiple & praDequeDetail_PackedExtraMask(extra_bits)));
    return main_being_multiple | extra;
}

//domain functions
//return bit limitation of maximal size calculating maximal how much objects of size rounded up to next power of two can be placed in half of address space
static inline int praDequeDetail_MaxSizeBitsUpToDeg2InHalfAddressSpace(size_t value_size)
{
    return kPradequeDetailAddressBits - 1 - praDequeDetail_UpperLog2(value_size);
}

//Calculates shift to use for calculation value_size specific small block sizes from default small block size.
static inline int praDequeDetail_ShiftForSmallBlockSizes(size_t value_size)
{
    //*2 corresponds to single allocation of two smallest blocks.
    const int small_blocks_in_optimal_bytes = kPradequeDetailFirstAllocationMaxOptimalBytes / (value_size * kPradequeDetailHalfTableSmallSpecial0MinSize * 2);
    return praDequeDetail_LowerLog2(small_blocks_in_optimal_bytes);
}
//Calculates bits required to adress value in all small block.
static inline int praDequeDetail_TotalHalfSmallBlocksAdressBits(size_t value_size)
{
    return kPradequeDetailHalfTableSmallSpecialTotalMinBits + praDequeDetail_ShiftForSmallBlockSizes(value_size);
}
//API implementation
#ifndef PRA_DEQUE_DETAIL_API
#define PRA_DEQUE_DETAIL_API inline
#endif

PRA_DEQUE_DETAIL_API
void pradeque_clear(pradeque_t* deque, pradeque_params_t* params){}

PRA_DEQUE_DETAIL_API
intptr_t pradeque_max_size(size_t value_size)
{
    const int max_bits_size = kPradequeDetailSizeBits;//bits used for size storage
    const int max_bits_heap = praDequeDetail_MaxSizeBitsUpToDeg2InHalfAddressSpace(value_size);//maximal allocatable array is limited by address space
    const int max_bits_table = praDequeDetail_TotalHalfSmallBlocksAdressBits(value_size) + kPradequeDetailLog2OfCapacityGrowByAddingAllGroupedBy3 + 1;//log2 of element count that can be placed in all blocks in table. +1 corresponds to table consisting of two halves.
	
	//first calculate max size bitness, then calculate size itself
    const int max_heap_table = max_bits_heap > max_bits_table ? max_bits_heap : max_bits_table;
    const int max_all = max_bits_size > max_heap_table ? max_bits_size : max_heap_table;
	return (((intptr_t)1) << max_all) - 1;
}