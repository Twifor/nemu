#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "movsb-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "movsb-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(movsb_rm2r)
