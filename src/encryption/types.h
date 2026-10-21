#ifndef TYPES_H
#define TYPES_H

#include "../libs/tiny-bignum-c/bn.h"

namespace Encryption {
    struct int256 {
        DTYPE array[BN_ARRAY_SIZE];
    };
}

#endif