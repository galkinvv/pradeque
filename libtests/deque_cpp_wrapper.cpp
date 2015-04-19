#include "pradeque.hpp"
#include <gtest/gtest.h>

struct S
{
    typedef const char cchar1_t[1];
    cchar1_t& data_;
    explicit S(cchar1_t& data):data_(data){}

    S& operator=(const S&) = delete;
    S(const S&) = delete;
    S(S&&) = delete;
};


TEST(deque_core, cpp_api)
{
	typedef PraDeque<S> Deque;
    S s{""};
}
