#pragma once
#include <stdint.h>
#include <assert.h>

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
