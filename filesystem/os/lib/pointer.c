#include "bmp.h"
#include "pointer.h"
#include "../os_init.h"
void pointer_init(uint8_t* picon){
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)VBE_INFO_LOC;
    uint32_t temp[5];
    temp[0]=(uint32_t)picon;
    syscall(6, temp);
    syscall(26, temp);
    temp[1]=temp[0];
    temp[0]=(uint32_t)picon;
    syscall(3, temp);
    struct bmp_head* bmp_file=(struct bmp_head*)temp[1];
    uint32_t frame_buff=get_scratchpad();
    temp[0]=vbe_info->width/2;
    temp[1]=vbe_info->height/2;
    temp[4]=0;
    display_bitmap(bmp_file, temp, frame_buff);
    set_vesa_frame();
}