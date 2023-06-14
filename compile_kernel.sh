cd kernel
gcc -fno-PIC -fno-stack-protector -m32 -s -c -o kernel.o kernel.c
nasm program_entry.asm -f elf32 -o program_entry.o
nasm assembly_instructs.asm -f elf32 -o assembly_instructs.o
nasm fat.asm -o fat.bin
cd boot
nasm multi_core_boot.asm -o multi_core_boot.bin
nasm ap_code.asm -o ap_code.bin
nasm jumper_aid.asm -f elf32 -o jumper_aid.o
gcc -fno-PIC -fno-stack-protector -m32 -s -c -o jumper.o jumper.c
cd ../../test
gcc -fno-PIC -fno-stack-protector -m32 -s -c -o prg.o prg.c
gcc -fno-PIC -fno-stack-protector -m32 -s -c -o prg_aid.o prg_aid.c
cd ..