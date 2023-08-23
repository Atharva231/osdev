#include "bmp.h"
uint32_t calc_down_scale(uint32_t orig, uint32_t resize){
    uint32_t n1=orig, n2=orig, c1=0, c2=0;
    uint32_t div=orig-resize;
    while(n1%div>0){
        n1++;
        c1++;
    }
    while(n2%div>0){
        n2--;
        c2++;
    }
    if(c1>=c2){
        return n2/div;
    }
    else{
        return n1/div;
    }
}
uint32_t calc_up_scale(uint32_t orig, uint32_t resize){
    uint32_t n1=resize, n2=resize, c1=0, c2=0;
    uint32_t div=resize-orig;
    while(n1%div>0){
        n1++;
        c1++;
    }
    while(n2%div>0){
        n2--;
        c2++;
    }
    if(c1>=c2){
        return n2/div;
    }
    else{
        return n1/div;
    }
}
void set_bmp_frame(struct vesa_frame* data){
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)VBE_INFO_LOC;
    uint32_t src_ptr, offset, pad;
    uint32_t x_scale, y_scale;
    uint32_t img_Bpp=data->img_bpp/8, screen_Bpp=data->screen_bpp/8;
    uint8_t* vesa=(uint8_t*)data->frame_buff;
    uint8_t* src=(uint8_t*)data->image;
    bool f=false;
    pad=4-((data->img_width*img_Bpp)%4);
    if(pad==4){
        pad=0;
    }
    if(screen_Bpp > img_Bpp){
        f=true;
    }
    if(data->scale==0){
        if(data->x_offset<0 || data->y_offset<0){
            return;
        }
        for(uint32_t y=0; (y<data->img_height)&&((y+data->y_offset)<vbe_info->height); y++){
            src_ptr=(data->img_height-y-1)*data->img_width*img_Bpp + ((data->img_height-y-1)*pad);
            for(uint32_t x=0; (x<data->img_width)&&((x+data->x_offset)<vbe_info->width); x++){
                offset = data->pitch*(y+data->y_offset) + (data->x_offset+x)*screen_Bpp;
                for(uint8_t i=0; i<img_Bpp; i++){
                    vesa[offset+i]=src[src_ptr+i];
                }
                if(f){
                    vesa[offset + (screen_Bpp-1)]=0x00;
                }
                src_ptr+=img_Bpp;
            }
        }
    }
    else if(data->scale>0){
        uint32_t t[screen_Bpp];
        x_scale=calc_up_scale(data->img_width, data->resize_width);
        y_scale=calc_up_scale(data->img_height, data->resize_height);
        uint32_t prev, next;
        for(uint32_t y=0, y1=0; y<data->img_height; y++, y1++){
            src_ptr=(data->img_height-y-1)*data->img_width*img_Bpp + ((data->img_height-y-1)*pad);
            for(uint32_t x=0, x1=0; x<data->img_width; x++, x1++){
                offset = data->pitch*(y1+data->y_offset) + (data->x_offset+x1)*screen_Bpp;
                for(uint8_t i=0; i<img_Bpp; i++){
                    vesa[offset+i]=src[src_ptr+i];
                }
                if(f){
                    vesa[offset + (screen_Bpp-1)]=0x00;
                }
                if(x%x_scale==0 && x>0 && x<data->img_width-1){
                    ++x1;
                    offset = data->pitch*(y1+data->y_offset) + (data->x_offset+x1)*screen_Bpp;
                    for(uint8_t i=0; i<img_Bpp; i++){
                        vesa[offset+i]=(src[src_ptr+i] + src[src_ptr+img_Bpp+i])/2;
                    }
                    if(f){
                        vesa[offset + (screen_Bpp-1)]=0x00;
                    }
                }
                src_ptr+=img_Bpp;
            }
            if(y%y_scale==0 && y>0 && y<data->img_height){
                ++y1;
                prev=(data->img_height-y-1)*data->img_width*img_Bpp + ((data->img_height-y-1)*pad);
                next=(data->img_height-y-2)*data->img_width*img_Bpp + ((data->img_height-y-2)*pad);
                for(uint32_t x=0, x1=0; x<data->img_width; x++, x1++){
                    offset = data->pitch*(y1+data->y_offset) + (data->x_offset+x1)*screen_Bpp;
                    for(uint8_t i=0; i<img_Bpp; i++){
                        vesa[offset+i]=(src[prev+i] + src[next+i])/2;
                    }
                    if(f){
                        vesa[offset + (screen_Bpp-1)]=0x00;
                    }
                    if(x%x_scale==0 && x>0 && x<data->img_width-1){
                        x1++;
                        offset = data->pitch*(y1+data->y_offset) + (data->x_offset+x1)*screen_Bpp;
                        for(uint8_t i=0; i<img_Bpp; i++){
                            t[i]=(src[(next-img_Bpp)+i] + src[(next+img_Bpp)+i])/2;
                        }
                        if(f){
                            t[(screen_Bpp-1)]=0x00;
                        }
                        for(uint8_t i=0; i<img_Bpp; i++){
                            vesa[offset+i]=(src[prev+i] + t[i])/2;
                        }
                        if(f){
                            vesa[offset + (screen_Bpp-1)]=0x00;
                        }
                    }
                    prev+=img_Bpp;
                    next+=img_Bpp;
                }

            }
        }
    }
    else if(data->scale<0){
        x_scale=calc_down_scale(data->img_width, data->resize_width);
        y_scale=calc_down_scale(data->img_height, data->resize_height);
        for(uint32_t y=0, y1=0; y<data->img_height; y++){
            if(y%y_scale==0){
                continue;
            }
            src_ptr=(data->img_height-y-1)*data->img_width*img_Bpp;
            src_ptr+=((data->img_height-y-1)*pad);
            for(uint32_t x=0, x1=0; x<data->img_width; x++){
                if(x%x_scale>0){
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
}

void display_bitmap(struct bmp_head* bmp_file, uint32_t* data, uint32_t framebuffer){
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)VBE_INFO_LOC;
    struct vesa_frame frame;
    uint32_t bytes_per_pixel=bmp_file->bits_per_pixel/8;
    frame.img_bpp=bmp_file->bits_per_pixel;
    frame.image=(uint32_t)bmp_file+bmp_file->data_offset;
    frame.img_width=bmp_file->width;
    frame.img_height=bmp_file->height;
    frame.pitch=vbe_info->pitch;
    frame.frame_buff=framebuffer;
    frame.screen_bpp=vbe_info->bpp;
    frame.x_offset=data[0];
    frame.y_offset=data[1];
    frame.resize_width=data[2];
    frame.resize_height=data[3];
    frame.scale=data[4];
    set_bmp_frame(&frame);
}