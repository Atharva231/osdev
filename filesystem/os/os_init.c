#include "os_init.h"
void syscall(uint16_t id, uint32_t* temp){
    void(*func_ptr)(uint32_t sysbuff)=(void*)*((uint32_t*)SHARED_MEM);
    uint32_t f[]={id, (uint32_t)temp};
    (*func_ptr)((uint32_t)f);
    asm("int $0x80");
    print_text("syscall");
}
void main(){
    clear_screen();
    uint32_t temp[4];
    uint8_t dir[]="os";
    temp[0]=(uint32_t)dir;
    syscall(11, temp);
    uint8_t buff[64];
    temp[0]=(uint32_t)buff;
    syscall(16, temp);
    print_text(buff);
}
