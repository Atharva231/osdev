#include <stdint.h>
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