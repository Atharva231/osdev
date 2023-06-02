void kmain();
void ap_code();
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
        ap_code();
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
#include "./drivers/mp_config.c"
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
uint8_t pc=1;
extern void halt();
void prg(){
    uint8_t str[]="Hello ";
    print_text(str);
    halt();
}
uint8_t lock=0;
bool p=true;
void ap_code(){
    while(lock>0);
    lock=1;
    while(p);
    print_text("AP");
    ap_idt_init();
    ap_lapic_init();
    pc+=1;
    lock=0;
    while(1);
}
void kmain(){
    clear_screen();
    idt_init();
    kb_init();
    heap_init(0xC00000, 0xCFFFFF);
    disk_init(0x10000, 0x100000);
    vmm_init(0xFFFFF);
    //filesystem_init(0x7C00);
    set_home_addr((uint32_t)prg);
    lapic_init();
    pit_timer_init();
    rtc_init();
    calib_lapic_timer();
    uint8_t string[]="Atharva ";
    uint8_t delim[]=",";
    print_text(string);
    uint32_t* k=(uint32_t*)get_syscall_buff();
    lock=0;
    p=false;
    for(uint8_t i=1;i<num_cores;i++){
        pc=0;
        call_id=i;
        while(pc==0);
        sleep_ms(100);
    }
    /*k[0]=22;
    uint8_t files[]="prg.c prg_aid.c ";
    k[1]=(uint32_t)&files[0];
    asm("int $0x80");
    struct Process_Control_Block* pcb=(struct Process_Control_Block*)k[0];
    pcb->pstat=2;
    exec_prg(pcb->entry_addr, pcb->esp);
    pcb->pstat=0;*/
    halt();
}
