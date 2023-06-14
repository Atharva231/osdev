void kmain();
void ap_init_code();
void set_call_id(char);
extern void get_msr(int code, int* data);
extern void set_msr(int code, int* data);
char call_id=0;

void start(){
    int msr[2];
    get_msr(0x1B, msr);
    int id=*((int*)((msr[0]&0xFFFFF000)|0x20));
    id = id>>24;
    if(id==0){
        kmain();
    }
    else{
        while(call_id!=id);
        ap_init_code();
    }
}

void set_call_id(char a){
    call_id=a;
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
void kmain(){
    clear_screen();
    idt_init();
    kb_init();
    heap_init(0xC00000, 0xDFFFFF);
    atapio_init(true);
    disk_init(0x10000, 0x100000);
    vmm_init(0xFFFFF);
    filesystem_init(0x9400);
    set_home_addr((uint32_t)prg);
    lapic_init();
    pit_timer_init();
    rtc_init();
    calib_lapic_timer();
    init_ap();
    syscall_init();
    print_text("Atharva ");
    syscall_buff[0]=22;
    uint8_t files[]="prg.c prg_aid.c ";
    syscall_buff[1]=(uint32_t)&files[0];
    self_intr(0x80);
    struct Process_Control_Block* pcb=(struct Process_Control_Block*)syscall_buff[0];
    pcb->pstat=2;
    exec_prg(pcb->entry_addr, pcb->esp);
    pcb->pstat=0;

    syscall_buff[0]=23;
    syscall_buff[1]=pcb->pid;
    self_intr(0x80);

    while(1){
        asm("hlt");
    }
}
