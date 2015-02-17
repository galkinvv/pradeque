#pragma once
#include "pradeque.h"
#include <limits>
#include <cstdint>

namespace pradeque_detail
{
using namespace std;

constexpr inline int GetLowerLog2(uint32_t i)
{
   //use macro-based oneliner
   return ;
}
template <class T, class TDifferenceType, int tLog2MaxSize, int tLog2BlockRatio>
class Core
{
public:
    typedef typename std::make_unsigned<TDifferenceType>::type size_type;
    typedef TDifferenceType difference_type;
private:
    struct Statics
	{
        // get array index by lower log2 of element index
        // can't be applied to element with index zero, so never returns 0
        static constexpr inline int ArrayIndex(int log2elemnent_index)
        {
            return 1 + (log2elemnent_index/tLog2BlockRatio);
        }

        static constexpr inline int LastArray()
        {
            // array containing elements with pre-max lower log2 is extended to contain max_size elements
            return ArrayIndex(tLog2MaxSize - 1);
        }

        static constexpr inline size_type Deg2(int degree)
        {
            return size_type(1)<<size_type(degree);
        }
        
        //sizes are calculated in assumption that:
        //  first array size is 1
        //    - this is for simplicity and efficient handling of case where it is pointer to first fixed-sized array
        //  every array except last ends in a way that sum of it's and previous array sizes is a some degree of 2
        //    - this is required to simplified determination of specific array from global index
        //  last array size is exactly 2**tLog2MaxSize 
        //    - this is required to correctly handle queue wraparound with max_size elements
        //  adjacent arrays have sizes with ratio near to 2**tLog2BlockRatio
        //    - this is required to control memory usage
		static constexpr inline size_type NthArraySize(int non_negative_array_index)
		{
            return (LastArray() == non_negative_array_index) ?
                Deg2(tLog2MaxSize):
                (Deg2(non_negative_array_index * tLog2BlockRatio) - (
                    (0 == non_negative_array_index) ?
                    size_type(0):
                    Deg2((non_negative_array_index - 1) * tLog2BlockRatio)
                )
            ;
		}
	};
public:
    class iterator
    {
       
    };
	template <class... Args>
    void emplace_front(Args&&... args)
    {
        //emplace_* methods are only ones having complex exception safety semantics
        //there are two points that can throw exceptions: memory allocation
        //and contained object construction.
        //Problem is solved by ensuring that the state is correct before any operation that can throw.
        //No any rollbacks are done, so presence of unused but allocated block treated as correct state.
        //First: we allocate memory and save a pointer in a table, now we own it.
        //We never need to allocate more than one memory block.
        //Second: we construct an object in allocated memory.
        //Third: we update bounds of container

    }
    
    template <class... Args>
    void emplace_back(Args&&... args){}
private:
    //ArrayTablePtr table;//auto pointer should handle move construction and calling function that release memory block of all table ethat
};
template <class T, uint32_t tRequiredSize>
class AlignedUninitializedArrayCore
{
    struct Statics
	{
		static const int kByteSizeUpperLog2 =GetUpperLog2(uint32_t(sizeof(T)) * tRequiredSize);
		static const int kByteSize = 1 << kByteSizeUpperLog2;
		static const int kItemSize = kByteSize/sizeof(T);
	};

  public:
	class iterator
	{
		//single-pointer iterator that allows both deferencing and getting pointer to container (using alignment)
		AlignedUninitializedArrayCore& container()
		{

		}
	};
	iterator begin()
	{

	};
	iterator end()
	{

	};
};

}