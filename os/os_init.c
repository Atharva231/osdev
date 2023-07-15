#include "os_init.h"
uint8_t buff[25];
void main(){
    clear_screen();
    void(*func_ptr)(uint32_t sysbuff)=(void*)*((uint32_t*)SHARED_MEM);
    uint32_t temp[4];
    uint32_t f[]={24, (uint32_t)temp};
    (*func_ptr)((uint32_t)f);
    asm("int $0x80");
    
    buff[0]=1;
}
