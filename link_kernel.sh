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
resize "./boot/jumper.bin"
cat ./boot/boot_bsp.bin ./boot/jumper.bin ./kernel.bin ./fat.bin ../test/prg.o ../test/prg_aid.o ./boot/boot_ap.bin > ../os.bin
cd ..
dd if=os.bin of=os.img
truncate -s 2M os.img
