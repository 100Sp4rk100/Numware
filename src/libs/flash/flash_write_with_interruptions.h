#ifndef FLASH_WRITE_WITH_INTERRUPTIONS_H
#define FLASH_WRITE_WITH_INTERRUPTIONS_H

#include <stddef.h>
#include <stdint.h>

#define SVC_ATTRIBUTES __attribute__((noinline, externally_visible))

namespace Flash {

bool EraseSectorWithInterruptions(int i, bool handleInterrupts);
bool WriteMemoryWithInterruptions(uint8_t* destination, const uint8_t* source,
                                  size_t length, bool handleInterrupts);

}  // namespace Flash

#endif
