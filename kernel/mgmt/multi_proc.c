#include <stdint.h>
uint32_t ap_task_addr;
void set_ap_task(uint32_t data, uint8_t dest_lapic_id){
    ap_task_addr=data;
    send_IPI(dest_lapic_id, 0x20);
}
uint32_t get_numcores(){
    uint32_t ebx=0, unused;
    __get_cpuid_count(0x0B, 0x01, &unused, &ebx, &unused, &unused);
    return ebx & 0xFFFF;
}

uint32_t get_thread_per_core(){
    uint32_t ebx=0, unused;
    __get_cpuid_count(0x0B, 0x00, &unused, &ebx, &unused, &unused);
    return ebx & 0xFFFF;
}
uint32_t ap_task(){
    //uint8_t str[]="AP_TASK!!";
    //print_text(str);
    return ap_task_addr;
}