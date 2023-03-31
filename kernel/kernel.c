void kmain();
void start(){
    kmain();
}
#include "./mgmt/heap.c"
#include "./drivers/screen.c"
#include "./drivers/timer.c"
#include "./drivers/keyboard.c"
#include "./drivers/disk.c"
#include "./drivers/pci.c"
#include "./mgmt/disk_mgmt.c"
#include "./mgmt/virtual_memory_mgmt.c"
#include "./mgmt/filesystem.c"
#include "./mgmt/process_mgmt.c"
#include "./mgmt/program_loader.c"
#include "./mgmt/system_call.c"
#include "./drivers/interrupts.c"
#include <stdint.h>
#include <stdbool.h>
#define SYSCALL_BUFFER_ADDR 0xFFFE0

extern uint32_t exec_prg(uint32_t addr, uint32_t base_ptr);
extern void halt();
void prg(){
    uint8_t str[]="Hello ";
    print_text(str);
    halt();
}
void kmain(){
    idt_init();
    kb_init((uint32_t)&prg);
    set_print_status(true);
    //set_pit_count(1000);
    //timer_init();
    heap_init(0xB00000, 0xC00000);
    clear_screen();
    disk_init(0x10000, 0x100000);
    vmm_init(0xFFFFF);
    filesystem_init(0x7600);
    uint8_t string[]="Atharva ";
    uint8_t delim[]=",";
    print_text(string);
    uint32_t* k=(uint32_t*)get_syscall_buff();
    uint32_t prg_addr=alloc_pages(0x10*0x200);
    read_sectors((uint16_t*)prg_addr, 0x3C, 0x10);
    uint32_t files[]={prg_addr+0x0, prg_addr+0x800, 0x0};
    k[0]=20;
    k[1]=(uint32_t)files;
    asm("int $0x80");
    //print_num(k[0]);
    exec_prg(k[0], k[1]);
    /*k[0]=22;
    uint8_t files[]="prg.c prg_aid.c ";
    k[1]=(uint32_t)&files[0];
    asm("int $0x80");
    struct Process_Control_Block* pcb=(struct Process_Control_Block*)k[0];
    print_num(pcb->entry_addr);*/
    /*uint8_t file_name[19]={0};
    file_name[0]='p';
    file_name[1]='r';
    file_name[2]='g';
    file_name[3]='.';
    file_name[4]='c';
    struct file_list_element* file=search_file(file_name);
    print_num(file->file_addr[0][0]);
    print_text(delim);
    print_num(file->file_addr[0][1]);
    print_text(delim);*/
    /*list_files((uint8_t*)file_name);
    print_text((uint8_t*)file_name);*/
    halt();
}