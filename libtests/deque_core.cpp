#include "pradeque.hpp"
#include <gtest/gtest.h>
#include <cstdint>
using namespace std;
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

TEST(deque_core, api)
{
    S s{""};
}

template <int tValue>
struct LowerLog2
{
	static const int kValue;
};

 template <int tValue>
 const int LowerLog2<tValue>::kValue = PRA_DEQUE_LOWER_LOG2_UINT32(tValue);

TEST(log2, macro_via_class)
{
	EXPECT_EQ(LowerLog2<   1>::kValue, 0);
    EXPECT_EQ(LowerLog2<   2>::kValue, 1);
    EXPECT_EQ(LowerLog2<   3>::kValue, 1);
	EXPECT_EQ(LowerLog2<   4>::kValue, 2);
    EXPECT_EQ(LowerLog2<   5>::kValue, 2);
    EXPECT_EQ(LowerLog2<   6>::kValue, 2);
    EXPECT_EQ(LowerLog2<   7>::kValue, 2);
    EXPECT_EQ(LowerLog2<   8>::kValue, 3);
    EXPECT_EQ(LowerLog2<   9>::kValue, 3);
    EXPECT_EQ(LowerLog2<0xA>::kValue, 3);
    EXPECT_EQ(LowerLog2<0xB>::kValue, 3);
    EXPECT_EQ(LowerLog2<0xC>::kValue, 3);
    EXPECT_EQ(LowerLog2<0xD>::kValue, 3);
    EXPECT_EQ(LowerLog2<0xE>::kValue, 3);
    EXPECT_EQ(LowerLog2<0xF>::kValue, 3);
    EXPECT_EQ(LowerLog2<16>::kValue, 4);
    EXPECT_EQ(LowerLog2<17>::kValue, 4);
}

TEST(log2, macro_64_direct)
{
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64(0xFFFFFFF), 7*4-1);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64(0x8000000), 7*4-1);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64(0x80000000ull), 8*4-1);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64(0x7FFFFFFFFFFFFFFF), 16*4-2);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64(0x8000000000000000ull), 16*4-1);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64(0xFFFFFFFFFFFFFFFF), 16*4-1);
}
