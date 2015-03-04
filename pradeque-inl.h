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

static inline int praDequeDetail_PackedExtraBits(size_t extra_value_range_size)
{
    assert(extra_value_range_size == (1u << PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(extra_value_range_size)));
    return PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(extra_value_range_size);
}

static inline uintptr_t praDequeDetail_PackedExtraMask(size_t extra_value_range_size)
{
    assert(extra_value_range_size);
    uintptr_t extra_mask = extra_value_range_size - 1;
    assert(!(extra_value_range_size & extra_mask));//ensures that size is a power of 2
    return extra_mask;
}

static inline uintptr_t praDequeDetail_PackedGetMainBeingMultiple(uintptr_t packed, size_t extra_value_range_size)
{
    return (packed & ~praDequeDetail_PackedExtraMask(extra_value_range_size));
}

static inline uintptr_t praDequeDetail_PackedGetExtra(uintptr_t packed, size_t extra_value_range_size)
{
    return packed & praDequeDetail_PackedExtraMask(extra_value_range_size);
}

static inline uintptr_t praDequeDetail_Packed(uintptr_t main_being_multiple, uintptr_t extra, size_t extra_value_range_size)
{
    assert(extra <= praDequeDetail_PackedExtraMask(extra_value_range_size));
    assert(!(main_being_multiple & praDequeDetail_PackedExtraMask(extra_value_range_size)));
    return main_being_multiple | extra;
}

static inline int praDequeDetail_LowerLog2(uint32_t i)
{
   return PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(i);
}



enum{
kPradequeUnusedPPVoidBits = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(sizeof(void*)),
 kPradequeAddressBits = sizeof(void*) * CHAR_BIT,

//count of bits required to adress element in a table of block entries
//the value should be smaller to minimize memory overhead for table allocation, which is done with quite small number of elements
//the value should be greatear to allow more effective buffer usage by allowing more buffer sizes
//the selected tecnbhique is to use 3 buffers of every size with sizes exponentially growing by 4
//because verry big and very smal containers are not used 32 table eentryes are enough to store one exponential sequence of blocks. It must be doubled for allowing both back and front exponential growth.
//so 6 bit is enough for 32 bit case.
//for doubling/halfing adddress space with the formula adds/subtracts a bit. This give nice result for current 64-bit systems (allowing up to 2^48 sizes) and meaningful result for 16 and 128 bit systems, so it looks good.

 kPradequeTableAddressBits = PRA_DEQUE_DETAIL_LOWER_LOG2_UINT32(kPradequeAddressBits)  + 1,
kPradequeSizeBits = kPradequeAddressBits - (kPradequeTableAddressBits - kPradequeUnusedPPVoidBits),

kPradequeHalfTableSize = 1 << (kPradequeTableAddressBits  - 1),
kPradequeHalfTableSmallSpecialEntrys = 2, //count of first element that does not correspond to standard size calculating formula.
//table size defines the relation between smallest and biggesst arrays.

};

#ifndef PRA_DEQUE_DETAIL_API
#define PRA_DEQUE_DETAIL_API inline
#endif

PRA_DEQUE_DETAIL_API
void pradeque_release(pradeque_t* deque, pradeque_params_t* params){}

PRA_DEQUE_DETAIL_API
intptr_t pradeque_max_size(pradeque_params_t* params)
{
    //size is limited by two factors: maximal allocatable array and number of bits used for size storage
    //maximal allocatable array is limited by address space and table size
	return 0;
}

