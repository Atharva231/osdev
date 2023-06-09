#include <stdint.h>
uint32_t ap_task_addr;
uint8_t lock=0;
uint8_t pc=0;
void ap_idt_init();

void set_ap_task(uint32_t func_addr, uint8_t dest_lapic_id){
    ap_task_addr=func_addr;
    send_IPI(dest_lapic_id, 0x20);
}
uint32_t get_numapics(){
    uint32_t ebx=0, unused;
    __get_cpuid(0x01, &unused, &ebx, &unused, &unused);
    return (ebx>>16) & 0xFF;
}

void ap_init_code(){
    while(lock>0);
    lock=1;
    print_text("AP");
    ap_idt_init();
    ap_lapic_init();
    send_EOI();
    ap_vmm_init();
    pc+=1;
    lock=0;
    while(1){
        asm("hlt");
    }
}

void init_ap(){
    lock=0;
    uint8_t procs=get_numapics();
    for(uint8_t i=1;i<procs;i++){
        pc=0;
        set_call_id(i);
        while(pc==0);
        sleep_us(50);
    }
}
uint32_t ap_task(){
    //uint8_t str[]="AP_TASK!!";
    //print_text(str);
    void(*func_ptr)(void)=(void*)ap_task_addr;
    (*func_ptr)();
    while(1){
        asm("hlt");
    }
}

