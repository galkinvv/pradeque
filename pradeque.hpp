#pragma once
#include <numeric_limits>
#include <cstdint>

namespace pradeque_impl
{
using namespace std;

constexpr inline int GetUpperLog2(uint32_t i)
{
   //use macro-based oneliner
   return 4;
}
template <class T, uint32_t tRequiredSize>
class aligned_uninitialized_array_core
{
    struct statics
	{
		static const int kByteSizeUpperLog2 =GetUpperLog2(uint32_t(sizeof(T)) * tRequiredSize);
		static const int kByteSize = 1 << kByteSizeUpperLog2;
		static const int kItemSize = kByteSize/sizeof(T);
	};

  public:
	template <class aligned_uninitialized_array_core>
	class aligned_array_iterator
	{
		//single-pointer iterator that allows both deferencing and getting pointer to container (using alignment)
		aligned_uninitialized_array_core& container()
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

template <class T, class TSize, int t_log2_max_size, int t_log2_block_ratio>
class core
{
	struct  statics
	{
		static constexpr TSize nth_array_elem_count(int positive_array_index)
		{

		}
	};
	//table of pointers to different-sized array must be aligmed in a way
	//allowing determine both pointer to table start and table index from single pointer.
public:

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
    ArrayTablePtr table;//auto pointer should handle move construction and calling function that release memory block of all table ethat
}
}
