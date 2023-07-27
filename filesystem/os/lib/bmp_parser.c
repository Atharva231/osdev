#include "../os_init.h"
void display_bitmap(uint8_t* file){
    uint32_t temp[4];
    temp[0]=(uint32_t)file;
    syscall(6, temp);
    syscall(26, temp);
    temp[1]=temp[0];
    temp[0]=(uint32_t)file;
    syscall(3, temp);
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)VBE_INFO_LOC;
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
}