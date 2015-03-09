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

enum{

kPradequeUnusedPPVoidBits = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(sizeof(void*)),
kPradequeAddressBits = sizeof(void*) * CHAR_BIT,

//count of bits required to adress element in a table of block entries
//the value should be smaller to minimize memory overhead for table allocation, which is done with quite small number of elements
//the value should be greatear to allow more effective buffer usage by allowing more buffer sizes
//the selected tecnbhique is to use 3 buffers of every size with sizes exponentially growing by 4
//because very big and very smal containers are not used 32 table eentryes are enough to store one exponential sequence of blocks. It must be doubled for allowing both back and front exponential growth.
//so 6 bit is enough for 32 bit case.
//for doubling/halfing adddress space with the formula adds/subtracts a bit. This give nice result for current 64-bit systems (allowing up to 2^48 sizes) and meaningful result for 16 and 128 bit systems, so it looks good.

kPradequeTableIndexBits = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(kPradequeAddressBits) + 1,

//limitation to size bits coming from storage scheme: pradeque_t stores in two intptr_t packed fields pointer to last table entry, first table entry index and size.
kPradequeSizeBits = 2 * kPradequeAddressBits - (kPradequeAddressBits - kPradequeUnusedPPVoidBits) - kPradequeTableIndexBits,

kPradequeHalfTableSize = 1 << (kPradequeTableIndexBits - 1),
kPradequeHalfTableSmallSpecialEntries = 2, //count of first element that does not correspond to standard size calculating formula.
kPradequeHalfTableSmallSpecial0MinSize = 1, //min size of special entry 0
kPradequeHalfTableSmallSpecial1MinSize = 3, //min size of special entry 1
kPradequeHalfTableSmallSpecialTotalMinBits = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(kPradequeHalfTableSmallSpecial0MinSize + kPradequeHalfTableSmallSpecial0MinSize), //min bitness of total elements in all small entries
kPradequeHalfTableSmallSpecialTotalMaxExtraBits = 5, //max possible extra bitness of all small entries

//table size defines the relation between smallest and biggesst arrays.
kPradequeHalfTableGroupedBy3Entries = (kPradequeHalfTableSize - kPradequeHalfTableSmallSpecialEntries) / 3,
kPradequeLog2OfCapacityGrowByAddingAllGroupedBy3 = 2 * kPradequeHalfTableGroupedBy3Entries,
//Max size addressed by all table blocks. Last +1 represents doubling when going from half table to full table
kPradequeLog2MaxFullTableCapacity = kPradequeHalfTableSmallSpecialTotalMinBits + kPradequeHalfTableSmallSpecialTotalMaxExtraBits + kPradequeLog2OfCapacityGrowByAddingAllGroupedBy3 + 1

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
    return kPradequeAddressBits - 1 - praDequeDetail_UpperLog2(value_size);
}
//API implementation
#ifndef PRA_DEQUE_DETAIL_API
#define PRA_DEQUE_DETAIL_API inline
#endif

PRA_DEQUE_DETAIL_API
void pradeque_clear(pradeque_t* deque, pradeque_params_t* params){}

PRA_DEQUE_DETAIL_API
intptr_t pradeque_max_size(pradeque_params_t* params)
{
    //size is limited by two factors: maximal allocatable array and number of bits used for size storage (kPradequeSizeBits)
    //maximal allocatable array is limited by address space (kPradequeAddressBits) and table size (estimate bitness of all small special and add kPradequeLog2OfCapacityGrowByAddingAllGroupedBy3)
	
	//first calculate max size bitness, then calculate size itself
	return 0;
}

