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
    int temp[5];
    strcpy(buff,"os");
    temp[0]=(uint32_t)buff;
    syscall(11, temp);
    strcpy(buff,"ui");
    temp[0]=(uint32_t)buff;
    syscall(11, temp);
    strcpy(buff, "char7.bmp");
    temp[0]=(uint32_t)buff;
    syscall(6, temp);
    syscall(26, temp);
    temp[1]=temp[0];
    temp[0]=(uint32_t)buff;
    syscall(3, temp);
    struct bmp_head* bmp_file=(struct bmp_head*)temp[1];
    temp[0]=50;
    temp[1]=50;
    temp[2]=15;
    temp[3]=bmp_file->height-2;
    temp[4]=0;
    display_bitmap(bmp_file, temp);
}
