#include "pradeque.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include <climits>
using namespace std;

extern "C" int count_errors_in_pradeque_plain_c_api();
TEST(deque_core, plain_c_api)
{
    EXPECT_EQ(count_errors_in_pradeque_plain_c_api(), 0);
}

template <size_t tValue>
struct MaxSize
{
	static constexpr pradeque_params_t kParams = PRADEQUE_PREPARED_PARAMS(tValue);
	static const int kLog2 = kParams.max_size_log2;
	static uint64_t Log2(){return kLog2;}
};

TEST(max_szie, macro_via_class)
{
	ASSERT_EQ(sizeof(size_t), sizeof(intptr_t)); //this test was implemented only for plain adress space
	const int kBitsInAllMemory = sizeof(intptr_t) * CHAR_BIT;
	EXPECT_LT(MaxSize<  1>::Log2(), kBitsInAllMemory);
}

