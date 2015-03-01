#include "pradeque.hpp"
#include "pradeque-inl.h"
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

TEST(deque_core, cpp_api)
{
    S s{""};
}

extern "C" int count_errors_in_pradeque_plain_c_api();
TEST(deque_core, plain_c_api)
{
    EXPECT_EQ(count_errors_in_pradeque_plain_c_api(), 0);
}

template <int tValue>
struct Log2
{
	static const int kLowerLog;
};

template <int tValue>
const int Log2<tValue>::kLowerLog = praDequeDetail_LowerLog2(tValue);

TEST(log2, func_via_class)
{
    EXPECT_EQ(Log2<   1>::kLowerLog, 0);
    EXPECT_EQ(Log2<   2>::kLowerLog, 1);
    EXPECT_EQ(Log2<   3>::kLowerLog, 1);
    EXPECT_EQ(Log2<   4>::kLowerLog, 2);
    EXPECT_EQ(Log2<   5>::kLowerLog, 2);
    EXPECT_EQ(Log2<   6>::kLowerLog, 2);
    EXPECT_EQ(Log2<   7>::kLowerLog, 2);
    EXPECT_EQ(Log2<   8>::kLowerLog, 3);
    EXPECT_EQ(Log2<   9>::kLowerLog, 3);
    EXPECT_EQ(Log2<0xA>::kLowerLog, 3);
    EXPECT_EQ(Log2<0xB>::kLowerLog, 3);
    EXPECT_EQ(Log2<0xC>::kLowerLog, 3);
    EXPECT_EQ(Log2<0xD>::kLowerLog, 3);
    EXPECT_EQ(Log2<0xE>::kLowerLog, 3);
    EXPECT_EQ(Log2<0xF>::kLowerLog, 3);
    EXPECT_EQ(Log2<16>::kLowerLog, 4);
    EXPECT_EQ(Log2<17>::kLowerLog, 4);
}

TEST(log2, macro_64_direct_big_values)
{
	EXPECT_EQ(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(0xFFFFFFF), 7*4-1);
	EXPECT_EQ(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(0x8000000), 7*4-1);
	EXPECT_EQ(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(0x80000000ull), 8*4-1);
	EXPECT_EQ(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(0x7FFFFFFFFFFFFFFF), 16*4-2);
	EXPECT_EQ(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(0x8000000000000000ull), 16*4-1);
	EXPECT_EQ(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(0xFFFFFFFFFFFFFFFF), 16*4-1);
}

TEST(log2, macro_64_direct_operation_priority)
{
	EXPECT_EQ(PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(2000 ? 7 : 2000), 2);
	EXPECT_EQ(-PRA_DEQUE_DETAIL_LOWER_LOG2_UINT64(0x80), -7);
}

