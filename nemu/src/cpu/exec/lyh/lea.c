#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "lea-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "lea-template.h"
#undef DATA_BYTE

make_helper_v(lea_no_rm2r)
