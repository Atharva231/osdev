cd kernel/boot
ld -melf_i386 -T link_jumper.ld -o jumper.bin --oformat binary
cd ..
ld -melf_i386 -T link_kernel.ld -o kernel.bin --oformat binary
cat ./boot/boot.bin ./boot/jumper.bin ./boot/gapper.bin ./kernel.bin ../test/prg.o ../test/prg_aid.o ./free_sectors.bin > ../os.bin
cd ..
dd if=os.bin of=os.img
#cp ./kernel/boot/boot.bin ./kernel/boot/jumper.bin ./kernel/kernel.bin imageFiles/
#mkisofs -o os.iso imageFiles/
