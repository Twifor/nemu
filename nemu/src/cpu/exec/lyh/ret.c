#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "ret-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "ret-template.h"
#undef DATA_BYTE

make_helper_v(ret_n)
make_helper_v(ret_i)
