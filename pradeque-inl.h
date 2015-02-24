#pragma once
#include <stdint.h>
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

static inline int praDequePackedExtraBits(size_t extra_max_value)
{
    assert(extra_max_value == (1 << PRA_DEQUE_LOWER_LOG2_UINT64(extra_max_value)));
    return PRA_DEQUE_LOWER_LOG2_UINT64(extra_max_value);
}

static inline uintptr_t praDequePackedExtraMask(size_t extra_max_value)
{
    return ~((~(uintptr_t)0) << praDequePackedExtraBits(extra_max_value));
}

static inline uintptr_t praDequePackedGetMainBeingMultiple(uintptr_t packed, size_t extra_max_value)
{
    return (packed_pointer & ~praDequePackedExtraMask(extra_max_value));
}

static inline uintptr_t praDequePackedGetExtra(uintptr_t packed, size_t extra_max_value)
{
    return packed_pointer & praDequePackedExtraMask(extra_max_value);
}

static inline uintptr_t praDequePacked(uintptr_t main_being_multiple, uintptr_t extra, size_t extra_max_value)
{
    assert(extra <= praDequePackedExtraMask(extra_max_value);
    assert(!(main_being_multiple & praDequePackedExtraMask(extra_max_value));
    return main_being_multiple | extra;
}

