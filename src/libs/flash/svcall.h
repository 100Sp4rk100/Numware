#ifndef SVCALL_H
#define SVCALL_H

namespace SVCall {

#define SVC_RETURNING_SINGLE_REGISTER(code, returnType, instruction) \
  returnType returnValue;                                            \
  asm volatile(instruction                                           \
               : [returnValue] "=r"(returnValue)                     \
               : [immediate] "I"(code)                               \
               : "r0", "r1", "r2", "r3");                            \
  return returnValue;

#define SVC_RETURNING_R0(code, returnType)        \
  SVC_RETURNING_SINGLE_REGISTER(code, returnType, \
                                "svc %[immediate] ; mov %[returnValue], r0")

#define SVC_FLASH_ERASE_SECTOR_WITH_INTERRUPTIONS 30
#define SVC_FLASH_WRITE_MEMORY_WITH_INTERRUPTIONS 32

}  // namespace SVCall

#endif
