void kmain();
void ap_init_code();
void set_call_id(char);
extern void get_msr(int code, int* data);
extern void set_msr(int code, int* data);
char call_id;

void start(){
    call_id=0;
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
#include "./drivers/vga.c"
#include "./drivers/pit_8254.c"
#include "./drivers/ps2_keyboard.c"
#include "./drivers/ps2_mouse.c"
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
#include "./mgmt/pci_device_mgmt.c"
#include "./mgmt/system_call.c"
#include "./drivers/interrupts.c"
#include <stdint.h>
#include <stdbool.h>

extern uint32_t exec_prg(uint32_t addr, uint32_t base_ptr);
extern void halt();
uint32_t shared_mem_ptr;
void prg(){
    uint8_t str[]="Hello ";
    print_text(str);
    halt();
}

void kmain(){
    shared_mem_ptr=0xDFF000;
    clear_screen();
    idt_init();
    kb_init();
    heap_init(0xC00000, 0xDFEFFF);
    atapio_init(true);
    disk_init(0x10000, 0x100000);
    vmm_init(0xFFFFF);
    set_prg_addr((uint32_t)resume_prg);
    lapic_init();
    pit_timer_init();
    rtc_init();
    calib_lapic_timer();
    init_ap();
    struct pci_device_list* pci_temp = (struct pci_device_list*)pci_init();
    //filesystem_init(0xAC00);
    syscall_init();
    *((uint32_t*)shared_mem_ptr)=(uint32_t)set_syscall_buff;
    shared_mem_ptr+=4;
    mouse_init((uint8_t*)shared_mem_ptr);
    shared_mem_ptr+=(1*4);
    print_text("Atharva ");
    /*uint32_t f_addr[2];
    uint32_t temp[4];
    struct file_list_element* f=search_file("prg.o");
    f_addr[0]=mem_alloc(f->file_addr[0][1]);
    read_file("prg.o", (uint8_t*)f_addr[0]);
    f=search_file("prg_aid.o");
    f_addr[1]=mem_alloc(f->file_addr[0][1]);
    read_file("prg_aid.o", (uint8_t*)f_addr[1]);
    temp[0]=(uint32_t)f_addr;
    uint32_t sysbuff[]={20, (uint32_t)temp};
    set_syscall_buff(sysbuff);
    self_intr(0x80);
    struct Process_Control_Block* pcb=(struct Process_Control_Block*)temp[0];
    pcb->pstat=2;
    exec_prg(pcb->entry_addr, pcb->stack_start);
    pcb->pstat=0;*/
    /*uint32_t f_addr[2];
    uint32_t temp[4];
    struct file_list_element* f=search_file("os_init.o");
    f_addr[0]=mem_alloc(f->file_addr[0][1]);
    read_file("os_init.o", (uint8_t*)f_addr[0]);
    chg_dir("lib");
    f=search_file("screen.o");
    f_addr[1]=mem_alloc(f->file_addr[0][1]);
    read_file("screen.o", (uint8_t*)f_addr[1]);
    temp[0]=(uint32_t)f_addr;
    uint32_t sysbuff[]={20, (uint32_t)temp};
    set_syscall_buff(sysbuff);
    self_intr(0x80);
    struct Process_Control_Block* pcb=(struct Process_Control_Block*)temp[0];
    pcb->pstat=2;
    //exec_prg(pcb->entry_addr, pcb->stack_start);
    pcb->pstat=0;*/
    halt();
}
