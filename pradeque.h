#pragma once
#include <stdlib.h>
#define LOWER_LOG2_UINT_HELPER(x,half_bits) (LOWER_LOG2_UINT##half_bits(x))
#define LOWER_LOG2_UINT8(x) (x>>1)
#define LOWER_LOG2_UINT16(x) LOWER_LOG2_UINT_HELPER((x),8)
#define LOWER_LOG2_UINT32(x) LOWER_LOG2_UINT_HELPER((x),16)
        LOWER_LOG2_UINT32(23)
constexpr inline int GetLowerLog2(uint32_t i)
typedef struct
{
    void* value;
    const void* table_entry_ptr_useful:(CHAR_BIT*sizeof(void*))-;
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
