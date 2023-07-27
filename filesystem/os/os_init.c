#include "os_init.h"
uint8_t buff[64];
void syscall(uint16_t id, uint32_t* temp){
    void(*func_ptr)(uint32_t sysbuff)=(void*)*((uint32_t*)SHARED_MEM);
    void(*self_intr)(uint32_t vector)=(void*)*((uint32_t*)SELF_INTR_FUNC);
    uint32_t f[]={id, (uint32_t)temp};
    (*func_ptr)((uint32_t)f);
    (*self_intr)(0x80);
}
void main(){
    clear_screen();
    uint32_t temp[4];
    strcpy(buff,"os");
    temp[0]=(uint32_t)buff;
    syscall(11, temp);
    strcpy(buff, "img.bmp");
    display_bitmap(buff);
}
