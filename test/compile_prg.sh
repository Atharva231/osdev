resize(){
	f_size=$(wc -c < "$1")
	f_size=$((f_size/512+1))
	f_size=$((f_size*512))
	truncate -s "$f_size" "$1"
}
gcc -fno-PIE -fno-stack-protector -m32 -s -c -o prg.o prg.c
gcc -fno-PIE -fno-stack-protector -m32 -s -c -o prg_aid.o prg_aid.c
resize "prg.o"
resize "prg_aid.o"