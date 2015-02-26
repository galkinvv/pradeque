#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define PRA_DEQUE_EMPTY
#define PRA_DEQUE_DEFER(id) id PRA_DEQUE_EMPTY
#define PRA_DEQUE_EXPAND(id) id

#define PRA_DEQUE_LOWER_LOG2_UINT_HELPER_WITH_SHIFT(x, half_bits, shift_width) (shift_width + (PRA_DEQUE_DEFER(PRA_DEQUE_LOWER_LOG2_UINT_HELPER##half_bits)((x >> shift_width))))

#define PRA_DEQUE_LOWER_LOG2_UINT_HELPER(x, half_bits) PRA_DEQUE_LOWER_LOG2_UINT_HELPER_WITH_SHIFT(x, half_bits, ((x >> half_bits) ? half_bits : 0))
#define PRA_DEQUE_LOWER_LOG2_UINT_HELPER2(x)  x>>1
#define PRA_DEQUE_LOWER_LOG2_UINT_HELPER4(x)  PRA_DEQUE_LOWER_LOG2_UINT_HELPER(x,2)
#define PRA_DEQUE_LOWER_LOG2_UINT_HELPER8(x)  PRA_DEQUE_LOWER_LOG2_UINT_HELPER(x,4)
#define PRA_DEQUE_LOWER_LOG2_UINT_HELPER16(x) PRA_DEQUE_LOWER_LOG2_UINT_HELPER(x,8)
#define PRA_DEQUE_LOWER_LOG2_UINT_HELPER32(x) PRA_DEQUE_LOWER_LOG2_UINT_HELPER(x,16)
#define PRA_DEQUE_LOWER_LOG2_UINT_HELPER64(x) PRA_DEQUE_LOWER_LOG2_UINT_HELPER(x,32)
#define PRA_DEQUE_LOWER_LOG2_UINT32(x) (PRA_DEQUE_EXPAND(PRA_DEQUE_EXPAND(PRA_DEQUE_EXPAND(PRA_DEQUE_EXPAND(PRA_DEQUE_LOWER_LOG2_UINT_HELPER32(((uint32_t)(x))))))))
#define PRA_DEQUE_LOWER_LOG2_UINT64(x) (PRA_DEQUE_EXPAND(PRA_DEQUE_EXPAND(PRA_DEQUE_EXPAND(PRA_DEQUE_EXPAND(PRA_DEQUE_EXPAND(PRA_DEQUE_LOWER_LOG2_UINT_HELPER64(((uint64_t)(x)))))))))

static inline int praDequePackedExtraBits(size_t extra_value_range_size)
{
    return PRA_DEQUE_LOWER_LOG2_UINT64(extra_value_range_size);
}

static inline uintptr_t praDequePackedExtraMask(size_t extra_value_range_size)
{
    assert(extra_value_range_size);
    uintptr_t extra_mask = extra_value_range_size - 1;
    assert(!(extra_value_range_size & extra_mask));//ensures that size is a power of 2
    return extra_mask;
}

static inline uintptr_t praDequePackedGetMainShifted(uintptr_t packed, size_t extra_value_range_size)
{
    return (packed & ~praDequePackedExtraMask(extra_value_range_size));
}

static inline uintptr_t praDequePackedGetExtra(uintptr_t packed, size_t extra_value_range_size)
{
    return packed & praDequePackedExtraMask(extra_value_range_size);
}

static inline uintptr_t praDequePacked(uintptr_t main_shifted, uintptr_t extra, size_t extra_value_range_size)
{
    assert(extra <= praDequePackedExtraMask(extra_value_range_size);
    assert(!(main_shifted & praDequePackedExtraMask(extra_value_range_size));
    return main_shifted | extra;
}

