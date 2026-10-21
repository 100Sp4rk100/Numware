#ifndef RSA_H
#define RSA_H

#include "types.h"

namespace Encryption{

class RSA {
public:
    static int256 uncrypt(int256 value, int256 n, int256 d);
    static int256 crypt(int256 value, int256 n, int256 e);

private:
    static int256 modPow(int256 base, int256 exp, int256 mod);
};

}

#endif