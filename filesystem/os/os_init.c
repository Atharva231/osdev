#include "os_init.h"
void main(){
    clear_screen();
    /*void(*func_ptr)(uint32_t sysbuff)=(void*)*((uint32_t*)SHARED_MEM);
    uint32_t temp[4];
    uint32_t f[]={24, (uint32_t)temp};
    (*func_ptr)((uint32_t)f);
    asm("int $0x80");*/
    while(1){
        set_cursor(0);
        print_num_hex(*((uint8_t*)0xDFF004));
        print_text(" ");
        print_num_hex(*((uint8_t*)0xDFF005));
        print_text(" ");
        print_num_hex(*((uint8_t*)0xDFF006));
        print_text(" ");
    }
}
