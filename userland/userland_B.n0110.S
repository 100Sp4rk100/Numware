.syntax unified
.cpu cortex-m4
.thumb

.section .userland_header, "a", %progbits
.byte 0xFE, 0xED, 0xC0, 0xDE
.ascii "25.2.0\0\0"
.word 0x200064A4
.word 0x0000A800
.word 0x90170000
.word 0x903F0000
.word 0x200117B4
.word 0x20037000
.word 0x903F0000
.word 0x903F0400
.byte 0xFE, 0xED, 0xC0, 0xDE

.section .isr_vector_table, "a", %progbits
.word 0x20008000
.word start + 1

.text
.global start
.type start, %function

start:
    @ for the moment we just do a "fake brick"
    b loop

    @ we call sectorAtAddress to find the sector adress to erase
    ldr r0, =address
    bl sectorAtAddress

    @ will execute DFU can help to write on the storage ?
    svc 54

    @ call erase sector
    movs r1, #0
    svc 30
    @cmp r0, #0
    @beq reset @ erase sector have failed (just for test)

    @svc 46 @ reset to enter in rescue

loop:
b loop

sectorAtAddress:
    @ Input : r0 = adress
    @ Output : r0 = sector number (or -1 if invalid)

    @ address -= ExternalFlashOrigin
    ldr r1, =ExternalFlashOrigin
    sub r0, r0, r1

    @ int i = address >> NumberOfAddressBitsIn64KbyteBlock
    ldr r1, =NumberOfAddressBitsIn64KbyteBlock
    lsr r2, r0, r1

    @ if (i > NumberOf64KSectors) return -1
    ldr r1, =NumberOf64KSectors
    cmp r2, r1
    bgt .L_sector_error

    @ if (i >= 1) return NumberOf4KSectors + NumberOf32KSectors + i - 1
    cmp r2, #1
    bge .L_sector_64k

    @ i = address >> NumberOfAddressBitsIn32KbyteBlock
    ldr r1, =NumberOfAddressBitsIn32KbyteBlock
    lsr r2, r0, r1

    @ if (i >= 1) return NumberOf4KSectors + i - 1
    cmp r2, #1
    bge .L_sector_32k

    @ i = address >> NumberOfAddressBitsIn4KbyteBlock
    ldr r1, =NumberOfAddressBitsIn4KbyteBlock
    lsr r0, r0, r1
    bx lr

.L_sector_64k:
    @ return NumberOf4KSectors + NumberOf32KSectors + i - 1
    ldr r0, =NumberOf4KSectors
    ldr r1, =NumberOf32KSectors
    add r0, r0, r1
    add r0, r0, r2
    sub r0, r0, #1
    bx lr

.L_sector_32k:
    @ return NumberOf4KSectors + i - 1
    ldr r0, =NumberOf4KSectors
    add r0, r0, r2
    sub r0, r0, #1
    bx lr

.L_sector_error:
    mov r0, #-1
    bx lr

reset:
    svc 46

.data
address = 0x90010000
ExternalFlashOrigin = 0x90000000
ExternalFlashLength = 0x800000
NumberOfAddressBitsIn4KbyteBlock = 0xC
NumberOfAddressBitsIn32KbyteBlock = 15
NumberOfAddressBitsIn64KbyteBlock = 0x10
NumberOf4KSectors = 0x8
NumberOf32KSectors = 0x1
NumberOf64KSectors = 0x7F