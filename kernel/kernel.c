void kmain();
void ap_init_code();
char call_id=0;
char num_cores=1;
void start(){
    int id=*((int*)0xFEE00020);
    id = id>>24;
    if(id==0){
        kmain();
    }
    else{
        ++num_cores;
        while(call_id!=id);
        ap_init_code();
    }
}
#include "./mgmt/heap.c"
#include "./drivers/screen.c"
#include "./drivers/pit_8254.c"
#include "./drivers/keyboard.c"
#include "./drivers/ata_pio.c"
#include "./drivers/pci.c"
#include "./drivers/rtc.c"
#include "./drivers/acpi.c"
#include "./drivers/ioapic.c"
#include "./drivers/lapic.c"
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

extern uint32_t exec_prg(uint32_t addr, uint32_t base_ptr);
extern void halt();
void prg(){
    uint8_t str[]="Hello ";
    print_text(str);
    halt();
}
void test_func(){
    uint32_t cr;
    asm("mov %%cr3, %0":"=r"(cr));
    print_num_hex(cr);
}
void kmain(){
    clear_screen();
    idt_init();
    kb_init();
    heap_init(0xC00000, 0xCFFFFF);
    disk_init(0x10000, 0x100000);
    vmm_init(0xFFFFF);
    //filesystem_init(0x9800);
    set_home_addr((uint32_t)prg);
    lapic_init();
    pit_timer_init();
    rtc_init();
    calib_lapic_timer();
    init_ap();
    uint8_t string[]="Atharva ";
    uint8_t delim[]=",";
    print_text(string);
    /*uint32_t* k=(uint32_t*)get_syscall_buff();
    k[0]=22;
    uint8_t files[]="prg.c prg_aid.c ";
    k[1]=(uint32_t)&files[0];
    self_intr(0x80);
    struct Process_Control_Block* pcb=(struct Process_Control_Block*)k[0];
    pcb->pstat=2;
    exec_prg(pcb->entry_addr, pcb->esp);
    pcb->pstat=0;*/

    while(1){
        asm("hlt");
    }
}
