#include "rsa.h"

namespace Encryption{

inline struct bn* to_bn(int256* x) {
    return reinterpret_cast<struct bn*>(x);
}

int256 RSA::modPow(int256 base_val, int256 exp_val, int256 mod_val) {
    int256 res, base, exp, mod, temp;

    bignum_init(to_bn(&res));
    bignum_from_int(to_bn(&res), 1);

    bignum_assign(to_bn(&base), to_bn(&base_val));
    bignum_assign(to_bn(&exp), to_bn(&exp_val));
    bignum_assign(to_bn(&mod), to_bn(&mod_val));

    bignum_mod(to_bn(&base), to_bn(&mod), to_bn(&base));

    while (!bignum_is_zero(to_bn(&exp))) {

        if (exp.array[0] & 1) {
            bignum_mul(to_bn(&res), to_bn(&base), to_bn(&temp));
            bignum_mod(to_bn(&temp), to_bn(&mod), to_bn(&res));
        }

        bignum_mul(to_bn(&base), to_bn(&base), to_bn(&temp));
        bignum_mod(to_bn(&temp), to_bn(&mod), to_bn(&base));

        bignum_rshift(to_bn(&exp), to_bn(&exp), 1);
    }

    return res;
}

int256 RSA::crypt(int256 value, int256 n, int256 e) {
    return modPow(value, e, n);
}

int256 RSA::uncrypt(int256 value, int256 n, int256 d) {
    return modPow(value, d, n);
}

}