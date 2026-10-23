#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include <stdint.h>

namespace Config {

constexpr int NumberOf4KSectors = 8;
constexpr int NumberOf32KSectors = 1;
constexpr int NumberOf64KSectors = 128 - 1;

constexpr uint8_t NumberOfAddressBitsIn64KbyteBlock = 16;
constexpr uint8_t NumberOfAddressBitsIn32KbyteBlock = 15;
constexpr uint8_t NumberOfAddressBitsIn4KbyteBlock = 12;

constexpr uint32_t ExternalFlashOrigin = 0x90000000;

}  // namespace Config

#endif
