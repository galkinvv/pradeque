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
	static const int kLowerLog = PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(tValue);
	static int LowerLog(){return kLowerLog;}
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
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0x7FFFFFFFFFFFFFFFll), 16*4-2);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0x8000000000000000ull), 16*4-1);
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0xFFFFFFFFFFFFFFFFll), 16*4-1);
}

TEST(log2, macro_64_direct_operation_priority)
{
	EXPECT_EQ(PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(2000 ? 7 : 2000), 2);
	EXPECT_EQ(-PRA_DEQUE_LOWER_LOG2_UINT64_DETAILV1(0x80), -7);
}

template <uint64_t tValue>
struct ZCount
{
	static const int kZeroes = PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(tValue);
	static int Zeroes(){return kZeroes;}
};

TEST(zero_count, func_via_class)
{
    EXPECT_EQ(ZCount<  1>::Zeroes(), 0);
    EXPECT_EQ(ZCount<  2>::Zeroes(), 1);
    EXPECT_EQ(ZCount<  3>::Zeroes(), 0);
    EXPECT_EQ(ZCount<  4>::Zeroes(), 2);
    EXPECT_EQ(ZCount<  5>::Zeroes(), 0);
    EXPECT_EQ(ZCount<  6>::Zeroes(), 1);
    EXPECT_EQ(ZCount<  7>::Zeroes(), 0);
    EXPECT_EQ(ZCount<  8>::Zeroes(), 3);
    EXPECT_EQ(ZCount<  9>::Zeroes(), 0);
    EXPECT_EQ(ZCount<0xA>::Zeroes(), 1);
    EXPECT_EQ(ZCount<0xB>::Zeroes(), 0);
    EXPECT_EQ(ZCount<0xC>::Zeroes(), 2);
    EXPECT_EQ(ZCount<0xD>::Zeroes(), 0);
    EXPECT_EQ(ZCount<0xE>::Zeroes(), 1);
    EXPECT_EQ(ZCount<0xF>::Zeroes(), 0);
    EXPECT_EQ(ZCount<16>::Zeroes(), 4);
    EXPECT_EQ(ZCount<17>::Zeroes(), 0);
}

TEST(zero_count, macro_64_direct_big_values)
{
	EXPECT_EQ(PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(0xFFFFFFF), 0);
	EXPECT_EQ(PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(0x8000000), 6*4+3);
	EXPECT_EQ(PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(0x80000000ull), 8*4-1);
	EXPECT_EQ(PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(0x7FFFFFFFFFFFFFFFll), 0);
	EXPECT_EQ(PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(0x8000000000000000ull), 16*4-1);
	EXPECT_EQ(PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(0xFFFFFFFFFFFFFFFFll), 0);
}

TEST(zero_count, macro_64_direct_operation_priority)
{
	EXPECT_EQ(PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(2000 ? 128*3 : 2000), 7);
	EXPECT_EQ(-PRA_DEQUE_LOW_ZEROES_UINT64_DETAILV1(0x80), -7);
}


template <uint64_t tValue>
struct DSolve
{
	static const uint64_t kSolution = PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(tValue);
	static uint64_t Solution(){return kSolution;}
};

bool checkCorrectDiofantioanSolution(uint64_t value, uint64_t solution)
{
	return (solution * value) == ((uint64_t)1);
};
bool checkCorrectDiofantioanSolving(uint64_t value)
{
	return checkCorrectDiofantioanSolution(value, PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(value));
};

TEST(diofantiane_solve, func_via_class)
{
    EXPECT_EQ(DSolve<  1>::Solution(), 1);
    EXPECT_EQ(DSolve<  3>::Solution(),0xaaaaaaaaaaaaaaabULL);
	EXPECT_PRED1(checkCorrectDiofantioanSolving, 5);
	EXPECT_PRED1(checkCorrectDiofantioanSolving, 7);
	EXPECT_PRED1(checkCorrectDiofantioanSolving, 9);
	EXPECT_PRED1(checkCorrectDiofantioanSolving, 11);
	EXPECT_PRED1(checkCorrectDiofantioanSolving, 13);
	EXPECT_PRED1(checkCorrectDiofantioanSolving, 15);
	EXPECT_PRED1(checkCorrectDiofantioanSolving, 17);
}

TEST(diofantiane_solve, macro_64_direct_big_values)
{
	EXPECT_PRED2(checkCorrectDiofantioanSolution, PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(0xFFFFFFF), 0xFFFFFFF);
	EXPECT_PRED2(checkCorrectDiofantioanSolution, PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(0x8000001), 0x8000001);
	EXPECT_PRED2(checkCorrectDiofantioanSolution, PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(0x80000001ull), 0x80000001ull);
	EXPECT_PRED2(checkCorrectDiofantioanSolution, PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(0x7FFFFFFFFFFFFFFFll), 0x7FFFFFFFFFFFFFFFll);
	EXPECT_PRED2(checkCorrectDiofantioanSolution, PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(0x8000000000000001ull), 0x8000000000000001ull);
	EXPECT_PRED2(checkCorrectDiofantioanSolution, PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(0xFFFFFFFFFFFFFFFFll), 0xFFFFFFFFFFFFFFFFll);
}

TEST(diofantiane_solve, macro_64_direct_operation_priority)
{
	EXPECT_EQ(PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(2000 ? 3 : 2000), 0xaaaaaaaaaaaaaaabULL);
	EXPECT_EQ(~PRA_DEQUE_DIOFANTEINE_SOLVE_UINT64_DETAILV1(3), ~0xaaaaaaaaaaaaaaabULL);
}

