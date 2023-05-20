void kmain();
void start(){
    kmain();
}
#include "./mgmt/heap.c"
#include "./drivers/screen.c"
#include "./drivers/timer.c"
#include "./drivers/keyboard.c"
#include "./drivers/ata_pio.c"
#include "./drivers/pci.c"
#include "./drivers/rtc.c"
#include "./drivers/acpi.c"
#include "./drivers/ioapic.c"
#include "./mgmt/disk_mgmt.c"
#include "./mgmt/virtual_memory_mgmt.c"
#include "./mgmt/multi_proc.c"
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
    pic_init();
    kb_init();
    set_print_status(true);
    rtc_init();
    //set_pit_count(1000);
    //timer_init();
    heap_init(0xB00000, 0xC00000);
    clear_screen();
    disk_init(0x10000, 0x100000);
    vmm_init(0xFFFFF);
    //filesystem_init(0x7C00);
    set_home_addr((uint32_t)prg);
    uint8_t string[]="Atharva ";
    uint8_t delim[]=",";
    print_text(string);
    uint32_t* k=(uint32_t*)get_syscall_buff();
    /*k[0]=22;
    uint8_t files[]="prg.c prg_aid.c ";
    k[1]=(uint32_t)&files[0];
    asm("int $0x80");
    struct Process_Control_Block* pcb=(struct Process_Control_Block*)k[0];
    pcb->pstat=2;
    exec_prg(pcb->entry_addr, pcb->esp);
    pcb->pstat=0;*/
    /*uint8_t* ptr=(uint8_t*)0xb8000;
    uint8_t* ebda=(uint8_t*)0x400;
    for(uint16_t i=0,j=0;j<0x1000;i++,j+=2){
        ptr[j]=ebda[i];
    }*/
    acpi_init();
    halt();
}