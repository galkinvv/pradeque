#pragma once
#include <limits>
#include <cstdint>
#include <type_traits>

namespace pradeque_detail
{
using namespace std;

constexpr inline int GetUpperLog2(uint32_t i)
{
   //use macro-based oneliner
   return ;
}
template <class T, class TDiffernceType, int tLog2MaxSize, int tLog2BlockRatio>
class Core
{
public:
    typedef make_unsigned<TDifferenceType>::type size_type;
    typedef TDiffernceType  difference_type;
private:
    struct Statics
	{
        static constexpr inline size_type Deg2(int degree)
        {
            return size_type(1)<<size_type(degree);
        }
        
        //sizes are calculated in assumption that:
        //  first array size is 1
        //  every array except last ends on a some degree of 2
        //  last array size is exactly given degree of 2
        //  adjacent arrays have sizes with ratio equal to given degree of 2
		static constexpr inline size_type NthArraySize(int non_negative_array_index)
		{
            if (0 == non_negative_array_index)
            {
                return Deg2(0);
            }
            return Deg2(non_negative_array_index * tLog2BlockRatio)
                - Deg2((non_negative_array_index - 1) * tLog2BlockRatio);
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
