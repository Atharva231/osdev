void kmain();
void start(){
    kmain();
}
#include "./mgmt/memory.c"
#include "./drivers/screen.c"
#include "./drivers/timer.c"
#include "./drivers/keyboard.c"
#include "./drivers/disk.c"
#include "./mgmt/virtual_memory_mgmt.c"
#include "./mgmt/filesystem.c"
#include "./mgmt/program_loader.c"
#include "./mgmt/system_call.c"
#include "./drivers/interrupts.c"
#include <stdint.h>
#include <stdbool.h>
#define SYSCALL_BUFFER_ADDR 0xFFFE0

extern void jump(uint32_t addr);
extern void call_func(uint32_t addr);
extern void halt();
uint32_t k;
void kmain(){
    idt_init();
    kb_init();
    set_print_status(true);
    //set_pit_count(1000);
    //timer_init();
    heap_init(0x800000);
    vmm_init();
    //filesystem_init(uint32_t addr, uint32_t limit);
    uint8_t string[]="Atharva ";
    uint8_t delim[]=",";
    clear_screen();
    print_text(string);
    read_sectors((uint16_t*)0x700000, 0x2D, 0x10);
    uint32_t* k=(uint32_t*)get_syscall_buff();
    uint32_t files[]={0x70024C, 0x700850, 0x0};
    k[0]=20;
    k[1]=(uint32_t)files;
    k[2]=0x710000;
    asm("int $0x80");
    //print_num(k[0]);
    jump(k[0]);
    halt();
}