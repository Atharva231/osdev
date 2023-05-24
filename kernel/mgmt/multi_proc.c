#include <stdint.h>

void get_tsc_freq(){
    uint32_t eax=0, ebx=0, ecx=0, edx=0;
    uint32_t leaf = 0x06;
    asm("mov %0, %%eax"::"r"(leaf));
    asm("cpuid");
    asm("mov %%eax, %0":"=r"(eax));
    asm("mov %%ebx, %0":"=r"(ebx));
    asm("mov %%ecx, %0":"=r"(ecx));
    asm("mov %%edx, %0":"=r"(edx));
    print_num_hex(eax);
    print_text(" ");
    print_num_hex(ebx);
    print_text(" ");
    print_num_hex(ecx);
    print_text(" ");
    print_num_hex(edx);
    print_text(" ");

}