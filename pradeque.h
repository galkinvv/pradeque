#pragma once
#include <stdint.h>

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


#include <stdlib.h>
#include <stdio.h>


typedef struct
{
    void* value;
    //const void* table_entry_ptr_useful:(CHAR_BIT*sizeof(void*))-UPPER_LOG2_UINT32(sizeof(void*));
}
pradeque_iterator_t;

typedef struct
{
   pradeque_iterator_t begin;  
   void *post_last;
}
pradeque_t;

typedef struct
{
    size_t value_size;
}
pradeque_params_t;
