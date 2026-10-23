#include "flash_information.h"

#include <assert.h>
#include "external_flash.h"

namespace Flash {

int SectorAtAddress(uint32_t address) {
  address -= Config::ExternalFlashOrigin;
  int i = address >> Config::NumberOfAddressBitsIn64KbyteBlock;
  if (i > Config::NumberOf64KSectors) {
    return -1;
  }
  if (i >= 1) {
    return Config::NumberOf4KSectors +
           Config::NumberOf32KSectors + i - 1;
  }
  i = address >> Config::NumberOfAddressBitsIn32KbyteBlock;
  if (i >= 1) {
    assert(i >= 0 && i <= Config::NumberOf32KSectors);
    i = Config::NumberOf4KSectors + i - 1;
    return i;
  }
  i = address >> Config::NumberOfAddressBitsIn4KbyteBlock;
  assert(i <= Config::NumberOf4KSectors);
  return i;
}

}  // namespace Flash
