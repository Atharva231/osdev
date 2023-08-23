#include "os_init.h"
#include "lib/screen.h"
#include "lib/pointer.h"
#include <string.h>
uint8_t buff[64];
void syscall(uint16_t id, uint32_t* temp){
    void(*func_ptr)(uint32_t sysbuff)=(void*)*((uint32_t*)SHARED_MEM);
    void(*self_intr)(uint32_t vector)=(void*)*((uint32_t*)SELF_INTR_FUNC);
    uint32_t f[]={id, (uint32_t)temp};
    (*func_ptr)((uint32_t)f);
    (*self_intr)(0x80);
}
void main(){
    scratchpad_init();
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)VBE_INFO_LOC;
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
    strcpy(buff,"pointer.bmp");
    pointer_init(buff);
    /*    
    uint8_t* pointer_coord=(uint8_t*)(SHARED_MEM+9);
    while(1){
        clear_scratchpad();
        temp[0]+=pointer_coord[0];
        temp[1]+=pointer_coord[1];
        temp[4]=0;
        display_bitmap(bmp_file, temp, frame_buff);
        set_vesa_frame();
    }*/
}
