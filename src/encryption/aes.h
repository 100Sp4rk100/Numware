#ifndef AES_H
#define AES_H

#include <stdint.h>

#include "types.h"

namespace Encryption{

class AES {
public:
    static int uncrypt(void * buffer, int size, int256 key);
    static void crypt(void * buffer, int size, int256 key);

private:
    static void SubBytes(uint8_t *block);
    static void ShiftRows(uint8_t *block);
    static void MixColumns(uint8_t *block);
    static void AddRoundKey(uint8_t *block, const uint8_t *key);
    static void KeyExpansion(uint8_t master_key[16], uint8_t round_keys[11][16]);
    static void InvSubBytes(uint8_t *block);
    static void InvShiftRows(uint8_t *block);
    static void InvMixColumns(uint8_t *block);

    static const uint8_t sbox[256];
    static const uint8_t inv_sbox[256];
    static const uint8_t Rcon[11];

    static constexpr uint8_t mix_matrix[16] = {
        2, 3, 1, 1,
        1, 2, 3, 1,
        1, 1, 2, 3,
        3, 1, 1, 2
    };
};

}

#endif