#if PRADEQUE_VERSION != 1
#error This file must be includede only indirectly from pradeque.h of correspodning version
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>

//TODO: fix broken UINT64_C for MSVC 2010 by replacing it with cast and shift

//The followin operations are needed as macro: lower log2, solve diofantine equation Y*2**constN + A*X = 1, find max degree of two that divides number.
//Lower log 2 as a macro implementation
#define PRA_DEQUE_LOG2_COMBINE_SUBRESULTS_DETAILV1(log2_xlow, log2_xhigh, x, halfxbits) ((x >> halfxbits) ? log2_xhigh + halfxbits : log2_xlow )

#define PRA_DEQUE_LOWER_LOG2_x02_DETAILV1(x) (x>>1)

#define PRA_DEQUE_LOWER_LOG2_x04_DETAILV1(x) PRA_DEQUE_LOG2_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOWER_LOG2_x02_DETAILV1((x & UINT64_C(0x0000000000000003))), PRA_DEQUE_LOWER_LOG2_x02_DETAILV1((x >> 0x02)), x, 0x02)
#define PRA_DEQUE_LOWER_LOG2_x08_DETAILV1(x) PRA_DEQUE_LOG2_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOWER_LOG2_x04_DETAILV1((x & UINT64_C(0x000000000000000F))), PRA_DEQUE_LOWER_LOG2_x04_DETAILV1((x >> 0x04)), x, 0x04)
#define PRA_DEQUE_LOWER_LOG2_x10_DETAILV1(x) PRA_DEQUE_LOG2_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOWER_LOG2_x08_DETAILV1((x & UINT64_C(0x00000000000000FF))), PRA_DEQUE_LOWER_LOG2_x08_DETAILV1((x >> 0x08)), x, 0x08)
#define PRA_DEQUE_LOWER_LOG2_x20_DETAILV1(x) PRA_DEQUE_LOG2_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOWER_LOG2_x10_DETAILV1((x & UINT64_C(0x000000000000FFFF))), PRA_DEQUE_LOWER_LOG2_x10_DETAILV1((x >> 0x10)), x, 0x10)
#define PRA_DEQUE_LOWER_LOG2_x40_DETAILV1(x) PRA_DEQUE_LOG2_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOWER_LOG2_x20_DETAILV1((x & UINT64_C(0x00000000FFFFFFFF))), PRA_DEQUE_LOWER_LOG2_x20_DETAILV1((x >> 0x20)), x, 0x20)

#define PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(x) (PRA_DEQUE_LOWER_LOG2_x40_DETAILV1((uint64_t)(x)))

//Count lower zero bits as macro implementation
#define PRA_DEQUE_LOW_ZEROES_COMBINE_SUBRESULTS_DETAILV1(zerocount_xlow, zerocount_xhigh, x, halfxbits) ((0 == (x & ~((~(uint64_t)(0)) << halfxbits))) ? zerocount_xhigh + halfxbits : zerocount_xlow )

//All functions gaurantee meaningful result only for non-zero x
#define PRA_DEQUE_LOW_ZEROES_x02_DETAILV1(x) (x == 2)

#define PRA_DEQUE_LOW_ZEROES_x04_DETAILV1(x) PRA_DEQUE_LOW_ZEROES_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOW_ZEROES_x02_DETAILV1((x & UINT64_C(0x0000000000000003))), PRA_DEQUE_LOW_ZEROES_x02_DETAILV1((x >> 0x02)), x, 0x02)
#define PRA_DEQUE_LOW_ZEROES_x08_DETAILV1(x) PRA_DEQUE_LOW_ZEROES_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOW_ZEROES_x04_DETAILV1((x & UINT64_C(0x000000000000000F))), PRA_DEQUE_LOW_ZEROES_x04_DETAILV1((x >> 0x04)), x, 0x04)
#define PRA_DEQUE_LOW_ZEROES_x10_DETAILV1(x) PRA_DEQUE_LOW_ZEROES_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOW_ZEROES_x08_DETAILV1((x & UINT64_C(0x00000000000000FF))), PRA_DEQUE_LOW_ZEROES_x08_DETAILV1((x >> 0x08)), x, 0x08)
#define PRA_DEQUE_LOW_ZEROES_x20_DETAILV1(x) PRA_DEQUE_LOW_ZEROES_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOW_ZEROES_x10_DETAILV1((x & UINT64_C(0x000000000000FFFF))), PRA_DEQUE_LOW_ZEROES_x10_DETAILV1((x >> 0x10)), x, 0x10)
#define PRA_DEQUE_LOW_ZEROES_x40_DETAILV1(x) PRA_DEQUE_LOW_ZEROES_COMBINE_SUBRESULTS_DETAILV1(PRA_DEQUE_LOW_ZEROES_x20_DETAILV1((x & UINT64_C(0x00000000FFFFFFFF))), PRA_DEQUE_LOW_ZEROES_x20_DETAILV1((x >> 0x20)), x, 0x20)

#define PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(x) (PRA_DEQUE_LOW_ZEROES_x40_DETAILV1((uint64_t)(x)))

//Find A that solves diofantine equation a*x == 1 (mod 2**64) for given odd number
//Solve the given equation for 2**(2*bits_solved) given solution for 2**(bits_solved). Assuming 2**bits_solved = N solution is: a + a * (-a*x mod N) = a + a* (1 - a*x ) = a* (2 - a *x)= a* (3 + ~(a*x)) (mod N^2)
#define PRA_DEQUE_DIOFANTEINE_SOLVE_NEXT_DEGREE_DETAILV1(a, x) (a * (3 + ~(a*x)))

//All functions gaurantee meaningful result only for odd x
#define PRA_DEQUE_DIOFANTEINE_SOLVE_x02_DETAILV1(x) (3+~x)

#define PRA_DEQUE_DIOFANTEINE_SOLVE_x04_DETAILV1(x) PRA_DEQUE_DIOFANTEINE_SOLVE_NEXT_DEGREE_DETAILV1(PRA_DEQUE_DIOFANTEINE_SOLVE_x02_DETAILV1(x), x)
#define PRA_DEQUE_DIOFANTEINE_SOLVE_x08_DETAILV1(x) PRA_DEQUE_DIOFANTEINE_SOLVE_NEXT_DEGREE_DETAILV1(PRA_DEQUE_DIOFANTEINE_SOLVE_x04_DETAILV1(x), x)
#define PRA_DEQUE_DIOFANTEINE_SOLVE_x10_DETAILV1(x) PRA_DEQUE_DIOFANTEINE_SOLVE_NEXT_DEGREE_DETAILV1(PRA_DEQUE_DIOFANTEINE_SOLVE_x08_DETAILV1(x), x)
#define PRA_DEQUE_DIOFANTEINE_SOLVE_x20_DETAILV1(x) PRA_DEQUE_DIOFANTEINE_SOLVE_NEXT_DEGREE_DETAILV1(PRA_DEQUE_DIOFANTEINE_SOLVE_x10_DETAILV1(x), x)
#define PRA_DEQUE_DIOFANTEINE_SOLVE_x40_DETAILV1(x) PRA_DEQUE_DIOFANTEINE_SOLVE_NEXT_DEGREE_DETAILV1(PRA_DEQUE_DIOFANTEINE_SOLVE_x20_DETAILV1(x), x)

#define PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(x) (PRA_DEQUE_DIOFANTEINE_SOLVE_x40_DETAILV1((uint64_t)(x)))

#define PRA_DEQUE_MIN_DETAILV1(x,y) (((x)<(y))?(x):(y))
#define PRA_DEQUE_MAX_DETAILV1(x,y) (((x)>(y))?(x):(y))

struct pradeque_detailV1
{
   struct {
	   void *first;
	   uintptr_t first_logical_index_packed_with_flags;
	   //stores first logical index and differnt flags describing container usage: current addressing mode constants, bidirectional usage presence, ...
	   //when becomes empty the table pointer is stored instead of first logical index

	   pradeque_iterator_t post_last;
   } detailV1; //pradeque_t details are not part of API
};

enum{

kPradequeDetailPointerAlign = PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(sizeof(void*)),
kPradequeDetailAddressBits = sizeof(void*) * CHAR_BIT,

//count of bits required to adress element in a table of block entries
//the value should be smaller to minimize memory overhead for table allocation, which is done with quite small number of elements
//the value should be greatear to allow more effective buffer usage by allowing more buffer sizes
//the selected tecnbhique is to use 3 buffers of every size with sizes exponentially growing by 4
//because very big and very smal containers are not used 32 table entryes are enough to store one exponential sequence of blocks. It must be doubled for allowing both back and front exponential growth.
//so 6 bit is enough for 32 bit case.
//for doubling/halfing adddress space with the formula adds/subtracts a bit. This give nice result for current 64-bit systems (allowing up to 2^48 sizes) and meaningful result for 16 and 128 bit systems, so it looks good.

kPradequeDetailFullTableIndexBits = PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(kPradequeDetailAddressBits) + 1,

kPradequeDetailHalfTableSize = 1 << (kPradequeDetailFullTableIndexBits - 1),
kPradequeDetailHalfTableSmallSpecialEntries = 1, //count of first element that does not correspond to standard size calculating formula.
kPradequeDetailHalfTableBigDupEntries = 4, //count of end entries with identical size.
//bits in table pointer containing mode info
kPradequeDetailBitsInTablePointerForModeDirection = 1, //backward or forward
kPradequeDetailBitsInTablePointerForModeShift = 2 + PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(kPradequeDetailHalfTableBigDupEntries - 1), //on both table sides there are 4*2 = 8 max blocks, they can be shifted by any value.
kPradequeDetailBitsInTablePointerForLastBigBlock = kPradequeDetailBitsInTablePointerForModeShift, //to be able calculate whether big blocks belong to pre-zero or to post-zero part.
//bits in every table pointer
kPradequeDetailBitsInEveryTablePointer = 2, //four possible values: not allocated, allocated but unused, used with plain address mode, used with non-plain address mode
kPradequeDetailBitsInSpecialTablePointer = kPradequeDetailBitsInEveryTablePointer + kPradequeDetailBitsInTablePointerForModeDirection + kPradequeDetailBitsInTablePointerForModeShift + kPradequeDetailBitsInTablePointerForLastBigBlock,
kPradequeDetailSpecialTablePointerPosition = 8, //shares the same cache line with table start and is expected to store quite big block
kPradequeDetailBlockAlignForNoTableMode = 1 + kPradequeDetailPointerAlign, //to store pointer pointer we need at least pointer size. Half-block is allocated for system information, so the block must have one more align.
kPradequeDetailMinBlockAlignToFitSpecial = kPradequeDetailBitsInSpecialTablePointer - 2 - ((kPradequeDetailSpecialTablePointerPosition - 2) / 3) * 2,//number of entirely fitted 3-same-block table entries.
kPradequeDetailMinBlockAlignToFitTableBits = PRA_DEQUE_MAX_DETAILV1(kPradequeDetailMinBlockAlignToFitSpecial, kPradequeDetailBitsInEveryTablePointer),
kPradequeDetailMinBlockAlign = PRA_DEQUE_MAX_DETAILV1(kPradequeDetailBlockAlignForNoTableMode, kPradequeDetailMinBlockAlignToFitTableBits),
kPradequeDetailFirstAllocationMinOptimalBytes = 64, //byte count being optimal for first allocation. Taken as one cache line. gcc std deque uses 512.

//table size defines the relation between smallest and biggesst arrays.
kPradequeDetailHalfTableGroupedBy3Entries = (kPradequeDetailHalfTableSize - kPradequeDetailHalfTableSmallSpecialEntries - kPradequeDetailHalfTableBigDupEntries) / 3,
kPradequeDetailLog2DiffBetweenSmallAndBigBlocks = 2 * (kPradequeDetailHalfTableGroupedBy3Entries - 1) + 2 + 2,//first +2 is for small->standard, second +2 is for standard->big
kPradequeDetailLog2DiffBetweenSmallBlocksAndMaxSize = kPradequeDetailLog2DiffBetweenSmallAndBigBlocks + PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(kPradequeDetailHalfTableBigDupEntries) + 1,//log2 of element count that can be placed in big blocks in table. +1 corresponds to table consisting of two halves.

kPradequeDetailLog2MaxContigousArraySize = sizeof(size_t) * CHAR_BIT,
kPradequeDetailLog2MaxContigousArraysForValueSize1 = PRA_DEQUE_MIN_DETAILV1(kPradequeDetailAddressBits -1, kPradequeDetailLog2MaxContigousArraySize + PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(kPradequeDetailHalfTableBigDupEntries) + 1),
//minimize count of elements if first block but get required alignment
kPradequeDetailLog2MaxAddressedElementsForValueAligned1 = kPradequeDetailLog2DiffBetweenSmallBlocksAndMaxSize + kPradequeDetailMinBlockAlign,
};

//utility functions - non-static functions

inline int praDequeDetail_LowerLog2(uintptr_t i)
{
   return PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(i);
}

inline int praDequeDetail_UpperLog2(uintptr_t i)
{
   return (i <= 1) ? 0 : 1 + praDequeDetail_LowerLog2(i - 1);
}

inline uintptr_t praDequeDetail_PackedExtraMask(int extra_bits)
{
    assert(extra_bits > 0);
    return ((uintptr_t)1 << extra_bits) - 1;
}

inline uintptr_t praDequeDetail_PackedGetMainBeingMultiple(uintptr_t packed, int extra_bits)
{
    return (packed & ~praDequeDetail_PackedExtraMask(extra_bits));
}

inline uintptr_t praDequeDetail_PackedSetMainBeingMultiple(uintptr_t main_being_multiple, uintptr_t packed, int extra_bits)
{
    assert(!(main_being_multiple & praDequeDetail_PackedExtraMask(extra_bits)));
    return main_being_multiple | (praDequeDetail_PackedExtraMask(extra_bits) & packed);//keep extra but change main
}

//domain functions
//return bit limitation of maximal size calculating maximal how much objects of size rounded up to next power of two can be placed in half of address space
inline int praDequeDetail_MaxSizeBitsUpToDeg2InHalfAddressSpace(size_t value_size)
{
    return kPradequeDetailAddressBits - 1 - praDequeDetail_UpperLog2(value_size);
}


//API implementation - functions are static inline to workaround conflicts of different versions
#define PRADEQUE_PREPARED_PARAMS_DETAILV1(size_t__value_size) \
/*size_t value_size*/ size_t__value_size, \
/*intptr_t max_size_log2=  min(max allocatable array for upper log2 of value size, max adressable by table array for value alignment)*/ PRA_DEQUE_MIN_DETAILV1( \
	kPradequeDetailLog2MaxContigousArraysForValueSize1 - (size_t__value_size ==1 ? 0 : (PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(size_t__value_size -1)+1)), \
	kPradequeDetailLog2MaxAddressedElementsForValueAligned1 - PRA_DEQUE_MIN_DETAILV1(PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(size_t__value_size), kPradequeDetailMinBlockAlign) \
),{ \
/*int size_aligned_bits*/PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(size_t__value_size), \
/*int small_block_entries_log2*/PRA_DEQUE_MAX_DETAILV1(kPradequeDetailMinBlockAlign - PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(size_t__value_size), 0), \
/*size_t odd_value_size*/size_t__value_size/(((size_t)1) << PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(size_t__value_size)), \
/*uintptr_t odd_mul_to_get_1_mod_max_block */ PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(size_t__value_size/(((size_t)1) << PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(size_t__value_size)))}

static inline void pradeque_clear(pradeque_t* deque, pradeque_params_t* params, pradeque_deallocator_t deallocator){/*TODO*/}

/*
static inline pradeque_params_t pradeque_prepare_params(size_t value_size)
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
*/
static inline void* pradeque_allocator_default(size_t alignment, size_t size, int zero_on_allocation, const pradeque_t* context)
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

static inline void pradeque_deallocator_default(void *block, size_t alignment, size_t size, int zero_on_allocation, const pradeque_t* context)
{
	assert(block);
	assert(size);
	assert(alignment);
	assert(0 == size % alignment);
	assert(!zero_on_allocation || size == (kPradequeDetailHalfTableSize * 2 * sizeof(void*)));
	assert(context);
	free(block);
}

static int kDradequeExplicitUseHelperDetailV1 = sizeof(&pradeque_prev_contigous_block) + sizeof (kDradequeExplicitUseHelperDetailV1);
