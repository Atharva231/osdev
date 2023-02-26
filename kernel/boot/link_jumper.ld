OUTPUT_FORMAT(elf32-i386)
INPUT(jumper.o jumper_aid.o)
SECTIONS
 {
   . = 0x7E00;
   .text : { *(.text) }
   .data : { *(.data) }
   .bss  : { *(.bss)  }
 }
