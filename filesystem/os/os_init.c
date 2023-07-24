#include "os_init.h"
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
    clear_screen();
    /*uint32_t temp[4];
    strcpy(buff,"os");
    temp[0]=(uint32_t)buff;
    syscall(11, temp);
    strcpy(buff, "img.ri");
    temp[0]=(uint32_t)buff;
    syscall(6, temp);
    syscall(26, temp);
    temp[1]=temp[0];
    strcpy(buff, "img.ri");
    temp[0]=(uint32_t)buff;
    syscall(3, temp);
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)0xA000;
    struct bmp_head* bmp_file=(struct bmp_head*)temp[1];
    struct vesa_frame frame;
    frame.bpp=0x18;
    frame.image=temp[1];
    frame.img_width=320;
    frame.img_height=200;
    frame.pitch=vbe_info->pitch;
    frame.frame_buff=vbe_info->framebuffer;
    set_vesa_frame(&frame);*/
    uint16_t off=*((uint16_t*)0xA00E);
    uint16_t seg=*((uint16_t*)0xA010);
    seg*=0x10;
    seg+=off;
    uint32_t d=seg+2;
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)0xA200;
    print_num_hex(vbe_info->width);
    print_num_hex(vbe_info->height);
    print_num_hex(vbe_info->bpp);
}
