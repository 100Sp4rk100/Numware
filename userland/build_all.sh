rm -r output
mkdir output

rm -r ../src/userland
mkdir ../src/userland

arm-none-eabi-gcc -mthumb -x assembler-with-cpp -c userland_A.n0110.S -o output/userland_A.n0110.o
arm-none-eabi-ld -T userland_A.n0110.ld -o output/userland_A.n0110.elf output/userland_A.n0110.o
arm-none-eabi-objcopy -O binary output/userland_A.n0110.elf output/userland_A.n0110.bin
xxd -i output/userland_A.n0110.bin > ../src/userland/userland_A_n0110.h

arm-none-eabi-gcc -mthumb -x assembler-with-cpp -c userland_B.n0110.S -o output/userland_B.n0110.o
arm-none-eabi-ld -T userland_B.n0110.ld -o output/userland_B.n0110.elf output/userland_B.n0110.o
arm-none-eabi-objcopy -O binary output/userland_B.n0110.elf output/userland_B.n0110.bin
xxd -i output/userland_B.n0110.bin > ../src/userland/userland_B_n0110.h

arm-none-eabi-gcc -mthumb -x assembler-with-cpp -c userland_A.n0120.S -o output/userland_A.n0120.o
arm-none-eabi-ld -T userland_A.n0120.ld -o output/userland_A.n0120.elf output/userland_A.n0120.o
arm-none-eabi-objcopy -O binary output/userland_A.n0120.elf output/userland_A.n0120.bin
xxd -i output/userland_A.n0120.bin > ../src/userland/userland_A_n0120.h

arm-none-eabi-gcc -mthumb -x assembler-with-cpp -c userland_B.n0120.S -o output/userland_B.n0120.o
arm-none-eabi-ld -T userland_B.n0120.ld -o output/userland_B.n0120.elf output/userland_B.n0120.o
arm-none-eabi-objcopy -O binary output/userland_B.n0120.elf output/userland_B.n0120.bin
xxd -i output/userland_B.n0120.bin > ../src/userland/userland_B_n0120.h