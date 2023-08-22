#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#define SELF_INTR_FUNC 0xDFF004
#define VBE_INFO_LOC 0xA200
struct vbe_mode_info {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));
struct __attribute__ ((packed)) vesa_frame{
	uint8_t screen_bpp;
    uint32_t frame_buff;
    uint16_t pitch;
    uint32_t x_offset;
    uint32_t y_offset;
    uint32_t image;
    uint32_t img_width;
    uint32_t img_height;
    uint8_t img_bpp;
	uint32_t resize_width;
	uint32_t resize_height;
	int scale;
};
struct __attribute__((__packed__)) bmp_head{
    uint8_t bmp_sig[2];
    uint32_t file_size;
    uint32_t reserved;
    uint32_t data_offset;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    uint32_t horiz_res;
    uint32_t vert_res;
    uint32_t colors;
    uint32_t imp_colors;
};
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
    uint32_t src_ptr, offset, pad;
    uint32_t x_scale, y_scale;
    uint32_t img_Bpp=data->img_bpp/8, screen_Bpp=data->screen_bpp/8;
    uint8_t* vesa=(uint8_t*)data->frame_buff;
    uint8_t* src=(uint8_t*)data->image;
    bool f=false;
    pad=4-((data->img_width*img_Bpp)%4);
    if(screen_Bpp > img_Bpp){
        f=true;
    }
    if(data->scale==0){
        for(uint32_t y=0; y<data->img_height; y++){
            src_ptr=(data->img_height-y-1)*data->img_width*img_Bpp;
            src_ptr+=((data->img_height-y-1)*pad);
            for(uint32_t x=0; x<data->img_width; x++){
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