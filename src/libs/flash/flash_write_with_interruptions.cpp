#include "svcall.h"
#include "flash_write_with_interruptions.h"

namespace Flash {

bool SVC_ATTRIBUTES EraseSectorWithInterruptions(int i, bool handleInterrupts) {
  SVC_RETURNING_R0(SVC_FLASH_ERASE_SECTOR_WITH_INTERRUPTIONS, bool)
}

bool SVC_ATTRIBUTES WriteMemoryWithInterruptions(uint8_t* destination,
                                                 const uint8_t* source,
                                                 size_t length,
                                                 bool handleInterrupts) {
  SVC_RETURNING_R0(SVC_FLASH_WRITE_MEMORY_WITH_INTERRUPTIONS, bool)
}

}  // namespace Flash