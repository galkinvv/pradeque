#include "pradeque.h"
typedef char my3chars[3];

const int a = (int x =3);
int count_errors_in_pradeque_plain_c_api()
{
	//TODO: make this static. so pradeque_prepare_params must become macro...
	pradeque_params_t my3chars_deque_params = pradeque_prepare_params(sizeof(my3chars));
	pradeque_t d = {};
	pradeque_clear(&d, &my3chars_deque_params, pradeque_deallocator_default);
	int error_count = 0;
	error_count += my3chars_deque_params.max_size <= 0;
	return error_count;
}
