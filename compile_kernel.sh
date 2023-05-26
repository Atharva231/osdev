cd kernel
gcc -fno-PIC -fno-stack-protector -m32 -s -c -o kernel.o kernel.c
nasm program_entry.asm -f elf32 -o program_entry.o
nasm assembly_instructs.asm -f elf32 -o assembly_instructs.o
nasm fat.asm -o fat.bin
cd boot
nasm boot_bsp.asm -o boot_bsp.bin
nasm boot_ap.asm -o boot_ap.bin
nasm jumper_aid.asm -f elf32 -o jumper_aid.o
gcc -fno-PIC -fno-stack-protector -m32 -s -c -o jumper.o jumper.c
cd ../..
