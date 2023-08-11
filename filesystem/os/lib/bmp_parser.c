#include "../os_init.h"
#include <math.h>

void set_bmp_frame(struct vesa_frame* data){
    uint32_t scale=1, src_ptr, offset, pad;
    uint32_t img_Bpp=data->img_bpp/8, screen_Bpp=data->screen_bpp/8;
    uint8_t* vesa=(uint8_t*)data->frame_buff;
    uint8_t* src=(uint8_t*)data->image;
    bool f=false;
    pad=4-((data->img_width*img_Bpp)%4);
    if(screen_Bpp > img_Bpp){
        f=true;
    }
    for(uint32_t y=0, y1=0; y<data->img_height; y++){
        if(y%scale>0){
            continue;
        }
        src_ptr=(data->img_height-y-1)*data->img_width*img_Bpp;
        src_ptr+=((data->img_height-y-1)*pad);
        for(uint32_t x=0, x1=0; x<data->img_width; x++){
            if(x%scale==0){
                offset = data->pitch*(y1+data->y_offset) + (data->x_offset+x1)*screen_Bpp;
                for(uint8_t i=0; i<img_Bpp; i++){
                    vesa[offset+i]=src[src_ptr+i];
                }
                if(f){
                    vesa[offset + (screen_Bpp-1)]=0x00;
                }
                x1++;
            }
            src_ptr+=img_Bpp;
        }
        y1++;
    }
}

void display_bitmap(uint8_t* file, uint32_t x_off, uint32_t y_off){
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
    uint32_t bytes_per_pixel=bmp_file->bits_per_pixel/8;
    frame.img_bpp=bmp_file->bits_per_pixel;
    frame.image=temp[1]+bmp_file->data_offset;
    frame.img_width=bmp_file->width;
    frame.img_height=bmp_file->height;
    frame.pitch=vbe_info->pitch;
    frame.frame_buff=vbe_info->framebuffer;
    frame.screen_bpp=vbe_info->bpp;
    frame.x_offset=x_off;
    frame.y_offset=y_off;
    set_bmp_frame(&frame);
}