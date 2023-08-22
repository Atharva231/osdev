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
    scratchpad_init();
    int temp[5];
    strcpy(buff,"os");
    temp[0]=(uint32_t)buff;
    syscall(11, temp);
    strcpy(buff,"ui");
    temp[0]=(uint32_t)buff;
    syscall(11, temp);
    strcpy(buff,"window");
    temp[0]=(uint32_t)buff;
    syscall(11, temp);
    strcpy(buff, "exit_button1.bmp");
    temp[0]=(uint32_t)buff;
    syscall(6, temp);
    syscall(26, temp);
    temp[1]=temp[0];
    temp[0]=(uint32_t)buff;
    syscall(3, temp);
    struct bmp_head* bmp_file=(struct bmp_head*)temp[1];
    uint32_t frame_buff=get_scratchpad();
    temp[0]=50;
    temp[1]=50;
    temp[2]=bmp_file->width*2;
    temp[3]=bmp_file->height*2;
    temp[4]=1;
    display_bitmap(bmp_file, temp, frame_buff);
    temp[0]=100;
    temp[1]=50;
    temp[4]=0;
    display_bitmap(bmp_file, temp, frame_buff);
    temp[0]=150;
    temp[1]=50;
    temp[2]=bmp_file->width/2;
    temp[3]=bmp_file->height/2;
    temp[4]=-1;
    display_bitmap(bmp_file, temp, frame_buff);
    set_vesa_frame();
}
