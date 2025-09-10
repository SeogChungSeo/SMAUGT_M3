
#include "reduce.h"

const uint32_t mask20 = ((uint64_t)1 << 20) - 1;

int32_t kred7340033(int64_t a)
{
    // Reduction modulo q
    int32_t c0, c1;
    int32_t ret = 0;
    int32_t c0_backup;

    c0 = (int32_t)(a & mask20);
    c1 = (int32_t)(a >> 20);

    // by ssc
    c0_backup = (c0 + c1);
    c0 <<= 3;
    ret = (c0 - c0_backup);
    //ret = 7 * c0 - c1;
    return (ret);
}
