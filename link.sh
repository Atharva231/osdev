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
resize "./boot/jumper.bin"
resize "./boot/ap_code.bin"
cat ./boot/multi_core_boot.bin ./boot/ap_code.bin ./boot/jumper.bin ./kernel.bin > ../temp.bin
cd ..
python3 create_fs_v2.py
cat temp.bin kernel/fat.bin files.bin > os.bin
dd if=os.bin of=os.img
truncate -s 5M os.img
