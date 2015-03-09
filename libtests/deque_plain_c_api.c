#include "pradeque.h"
typedef char my3chars[3];
static pradeque_params_t my3chars_deque_params = {sizeof(my3chars)};

int count_errors_in_pradeque_plain_c_api()
{
	pradeque_t d = {};
	pradeque_clear(&d, &my3chars_deque_params);
	return 0;
}

//inline implementation is included after all calls to be sure that they are compilable without it
#include "pradeque-inl.h"
