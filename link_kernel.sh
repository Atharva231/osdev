resize(){
	f_size=$(wc -c < "$1")
	f_size=$((f_size/512+1))
	f_size=$((f_size*512))
	truncate -s "$f_size" "$1"
}
cd kernel/boot
ld -melf_i386 -T link_jumper.ld -o jumper.bin --oformat binary
cd ..
ld -melf_i386 -T link_kernel.ld -o kernel.bin --oformat binary
resize "kernel.bin"
resize "fat.bin"
cat ./boot/boot.bin ./boot/jumper.bin ./boot/gapper.bin ./kernel.bin ./fat.bin ../test/prg.o ../test/prg_aid.o ./free_sectors.bin > ../os.bin
cd ..
dd if=os.bin of=os.img
#cp ./kernel/boot/boot.bin ./kernel/boot/jumper.bin ./kernel/kernel.bin imageFiles/
#mkisofs -o os.iso imageFiles/
