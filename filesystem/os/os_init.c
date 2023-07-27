#include "os_init.h"
#include <string.h>
#include <math.h>
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
    temp[0]=(uint32_t)buff;
    syscall(6, temp);
    syscall(26, temp);
    temp[1]=temp[0];
    strcpy(buff, "img.bmp");
    temp[0]=(uint32_t)buff;
    syscall(3, temp);
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)0xA200;
    struct bmp_head* bmp_file=(struct bmp_head*)temp[1];
    struct vesa_frame frame;
    frame.img_bpp=bmp_file->bits_per_pixel;
    frame.image=temp[1]+bmp_file->data_offset;
    frame.img_width=bmp_file->width;
    frame.img_height=bmp_file->height;
    frame.pitch=vbe_info->pitch;
    frame.frame_buff=vbe_info->framebuffer;
    frame.screen_bpp=vbe_info->bpp;
    set_vesa_frame(&frame);
    /*uint32_t temp[4];
    temp[0]=512*512*4;
    syscall(26, temp);
    uint8_t* img=(uint8_t*)temp[0];
    for(uint32_t i=0, len=512*512; i<len; i+=4){
        img[i]=0x00;
        img[i+1]=0x00;
        img[i+2]=0xFF;
        img[i+3]=0xFF;
        
    }
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)0xA200;
    struct vesa_frame frame;
    frame.img_bpp=0x20;
    frame.image=temp[0];
    frame.img_width=512;
    frame.img_height=512;
    frame.pitch=vbe_info->pitch;
    frame.frame_buff=vbe_info->framebuffer;
    frame.screen_bpp=vbe_info->bpp;
    set_vesa_frame(&frame);*/
    //print_num_hex(*((uint32_t*)(temp[0]+1)));
}
