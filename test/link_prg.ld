OUTPUT_FORMAT(elf32-i386)
ENTRY(main)
INPUT(prg.o)
SECTIONS
 {
   . = 0x800000;
   .text : { *(.text) }
   .data : { *(.data) }
   .bss  : { *(.bss)  }
 }
