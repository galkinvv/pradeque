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

