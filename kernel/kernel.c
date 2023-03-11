void kmain();
void start(){
    kmain();
}
#include "./mgmt/heap.c"
#include "./drivers/screen.c"
#include "./drivers/timer.c"
#include "./drivers/keyboard.c"
#include "./drivers/disk.c"
#include "./mgmt/disk_mgmt.c"
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
uint32_t k=1;
uint32_t test_func(){
    uint32_t addr;
    asm("mov 0x4(%%ebp), %%eax" : "=r"(addr));
    return addr;
}
void kmain(){
    idt_init();
    kb_init();
    set_print_status(true);
    //set_pit_count(1000);
    //timer_init();
    heap_init(0x800000, 0x900000);
    vmm_init(0xFFFFF);
    //filesystem_init(uint32_t addr, uint32_t limit);
    uint8_t string[]="Atharva ";
    uint8_t delim[]=",";
    clear_screen();
    print_text(string);
    uint32_t* k=(uint32_t*)get_syscall_buff();
    uint32_t prg_addr=alloc_pages(0x10*0x200);
    read_sectors((uint16_t*)prg_addr, 0x31, 0x10);
    uint32_t files[]={prg_addr+0x28C, prg_addr+0x890, 0x0};
    k[0]=20;
    k[1]=(uint32_t)files;
    k[2]=alloc_pages(600);
    asm("int $0x80");
    //print_num(k[0]);
    call_func(k[0]);
    halt();
}