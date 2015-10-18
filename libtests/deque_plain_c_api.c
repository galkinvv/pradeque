#include "pradeque.h"
typedef char my3chars[3];

static const pradeque_params_t my3chars_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(my3chars));
const pradeque_params_t size1_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(char[1]));
const pradeque_params_t size2_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(char[2]));
const pradeque_params_t size3_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(char[3]));
const pradeque_params_t size4_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(char[4]));
const pradeque_params_t size5_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(char[5]));
const pradeque_params_t size6_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(char[6]));
const pradeque_params_t size7_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(char[7]));
const pradeque_params_t size257_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(char[257]));
const pradeque_params_t size100000000_deque_params = PRADEQUE_PREPARED_PARAMS(sizeof(char[100000000]));

PRADEQUE_ALIGNED_UNION_WITH_LOCALITEMS(int, 8) gPradeque = PRADEQUE_INITIALIZER(gPradeque);

int count_errors_in_pradeque_plain_c_api()
{
	pradeque_t d = {};
	pradeque_clear(&d, &my3chars_deque_params, pradeque_deallocator_default);
	int error_count = 0;
	error_count += my3chars_deque_params.max_size_log2 <= 0;
	return error_count;
}
