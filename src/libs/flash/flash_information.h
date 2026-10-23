#ifndef DEVICE_FLASH_INFORMATION_H
#define DEVICE_FLASH_INFORMATION_H

#include <stdint.h>

namespace Flash {

int SectorAtAddress(uint32_t address);

}  // namespace Flash

#endif
