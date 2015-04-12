#include "pradeque.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include <type_traits>
using namespace std;
/*template <class T>
typename remove_reference<T>::type Val(T value)
{
	return value;
}*/

struct S
{
    typedef const char cchar1_t[1];
    cchar1_t& data_;
    explicit S(cchar1_t& data):data_(data){}

    S& operator=(const S&) = delete;
    S(const S&) = delete;
    S(S&&) = delete;
};

typedef pradeque_detail::Core<S, uint64_t, 62, 5> Core;

TEST(deque_core, cpp_api)
{
    S s{""};
}

extern "C" int count_errors_in_pradeque_plain_c_api();
TEST(deque_core, plain_c_api)
{
    EXPECT_EQ(count_errors_in_pradeque_plain_c_api(), 0);
}

template <uint64_t tValue>
struct Log2
{
	static int LowerLog(){return PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(tValue);}
	static int UpperLog(){return praDequeDetail_UpperLog2(tValue);}
};

TEST(log2, func_via_class)
{
    EXPECT_EQ(Log2<  1>::LowerLog(), 0);
    EXPECT_EQ(Log2<  1>::UpperLog(), 0);
    EXPECT_EQ(Log2<  2>::LowerLog(), 1);
    EXPECT_EQ(Log2<  2>::UpperLog(), 1);
    EXPECT_EQ(Log2<  3>::LowerLog(), 1);
    EXPECT_EQ(Log2<  3>::UpperLog(), 2);
    EXPECT_EQ(Log2<  4>::LowerLog(), 2);
    EXPECT_EQ(Log2<  4>::UpperLog(), 2);
    EXPECT_EQ(Log2<  5>::LowerLog(), 2);
    EXPECT_EQ(Log2<  5>::UpperLog(), 3);
    EXPECT_EQ(Log2<  6>::LowerLog(), 2);
    EXPECT_EQ(Log2<  6>::UpperLog(), 3);
    EXPECT_EQ(Log2<  7>::LowerLog(), 2);
    EXPECT_EQ(Log2<  7>::UpperLog(), 3);
    EXPECT_EQ(Log2<  8>::LowerLog(), 3);
    EXPECT_EQ(Log2<  8>::UpperLog(), 3);
    EXPECT_EQ(Log2<  9>::LowerLog(), 3);
    EXPECT_EQ(Log2<  9>::UpperLog(), 4);
    EXPECT_EQ(Log2<0xA>::LowerLog(), 3);
    EXPECT_EQ(Log2<0xA>::UpperLog(), 4);
    EXPECT_EQ(Log2<0xB>::LowerLog(), 3);
    EXPECT_EQ(Log2<0xB>::UpperLog(), 4);
    EXPECT_EQ(Log2<0xC>::LowerLog(), 3);
    EXPECT_EQ(Log2<0xC>::UpperLog(), 4);
    EXPECT_EQ(Log2<0xD>::LowerLog(), 3);
    EXPECT_EQ(Log2<0xD>::UpperLog(), 4);
    EXPECT_EQ(Log2<0xE>::LowerLog(), 3);
    EXPECT_EQ(Log2<0xE>::UpperLog(), 4);
    EXPECT_EQ(Log2<0xF>::LowerLog(), 3);
    EXPECT_EQ(Log2<0xF>::UpperLog(), 4);
    EXPECT_EQ(Log2<16>::LowerLog(), 4);
    EXPECT_EQ(Log2<16>::UpperLog(), 4);
    EXPECT_EQ(Log2<17>::LowerLog(), 4);
    EXPECT_EQ(Log2<17>::UpperLog(), 5);
}

TEST(log2, macro_64_direct_big_values)
{
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0xFFFFFFF), 7*4-1);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0x8000000), 7*4-1);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0x80000000ull), 8*4-1);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0x7FFFFFFFFFFFFFFF), 16*4-2);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0x8000000000000000ull), 16*4-1);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0xFFFFFFFFFFFFFFFF), 16*4-1);
}

TEST(log2, macro_64_direct_operation_priority)
{
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(2000 ? 7 : 2000), 2);
	EXPECT_EQ(-PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0x80), -7);
}

