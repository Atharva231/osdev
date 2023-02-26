OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
INPUT(kernel.o assembly_instructs.o)
SECTIONS
 {
   . = 0x100000;
   .text : { *(.text) }
   .data : { *(.data) }
   .bss  : { *(.bss)  }
 }
