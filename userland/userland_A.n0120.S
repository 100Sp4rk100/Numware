.syntax unified
.cpu cortex-m4
.thumb

.section .userland_header, "a", %progbits
.byte 0xFE, 0xED, 0xC0, 0xDE
.ascii "26.4.0\0\0"
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
    
loop:
b loop