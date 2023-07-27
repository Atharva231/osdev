#define SCREEN_START 0xb8000
#define SCREEN_END 0xb9000
#define IDT_SIZE 256
#define VBE_INFO_LOC 0xA200
#include <stdint.h>
#include <stdbool.h>
uint32_t cursor;
uint8_t* VGA;

struct __attribute__ ((packed)) vesa_frame{
    uint32_t frame_buff;
    uint32_t image;
    uint32_t img_width;
    uint32_t img_height;
    uint16_t pitch;
    uint8_t img_bpp;
	uint8_t screen_bpp;
};

void set_cursor(uint32_t cursor1){
    cursor=cursor1;
}
uint32_t print_text(uint8_t *str){
    uint8_t *pointer = (uint8_t*) (SCREEN_START + cursor);
    int i,j;
    for(i=0,j=0;str[i]!=0;i++,j+=2){
        pointer[j]=str[i];
    }
    cursor = j+cursor;
    return cursor;
}

uint32_t print_color_text(uint8_t *str, uint8_t color){
    uint8_t *pointer = (uint8_t*) (SCREEN_START + cursor);
    int i,j;
    for(i=0,j=0;str[i]!=0;i++,j+=2){
        pointer[j] = str[i];
        pointer[j+1] = color;
    }
    cursor = j+cursor;
    return cursor;
}

uint32_t print_num(uint32_t n){
    uint8_t *pointer = (uint8_t*) (SCREEN_START + cursor);
    int i=-1;
    uint32_t j=0;
    uint8_t t;
    uint8_t num_buff[10];
    while(n>0){
        t = n%10;
        i+=1;
        num_buff[i] = t+48;
        n/=10;
    }
    for(;i>-1;i--,j+=2){
        pointer[j]=num_buff[i];
    }
    cursor = j+cursor;
    return cursor;
}

uint32_t print_num_hex(uint32_t n){
    uint8_t preffix[]="0x";
    print_text(preffix);
    uint8_t *pointer = (uint8_t*) (SCREEN_START + cursor);
    int i=1;
    uint32_t j=0;
    uint8_t t;
    uint8_t num_buff[10];
    uint32_t temp;
    while(n!=0) {
		temp = n % 16;
		if( temp < 10)
		    temp =temp + 48; 
        else
		    temp = temp + 55;
		num_buff[i++]= temp;
		n = n / 16;
	}
    for(uint32_t k=i-1;k>0;k--,j+=2)
	    pointer[j]=num_buff[k];
    cursor = j+cursor;
    return cursor;
}
void clear_screen(){
    uint8_t *pointer = (uint8_t*)SCREEN_START;
    for(int i=0;i<0x1000;i+=2){
        pointer[i]=0;
    }
    cursor=0;
}

void set_vesa_frame(struct vesa_frame* data){
    uint32_t src_ptr,offset;
    bool f=false;
    if(data->screen_bpp > data->img_bpp){
        f=true;
    }
    uint8_t* vesa=(uint8_t*)data->frame_buff;
    uint8_t* src=(uint8_t*)data->image;
    for(uint32_t y=0; y<data->img_height; y++){
        src_ptr=(data->img_height-y-1)*data->img_width*(data->img_bpp/8);
        for(uint32_t x=0; x<data->img_width; x++){
            offset = data->pitch*y + x*(data->screen_bpp/8);
            for(uint8_t i=0; i<data->img_bpp/8; i++){
                vesa[offset+i]=src[src_ptr+i];
            }
            if(f){
                vesa[offset + ((data->screen_bpp/8)-1)]=0xFF;
            }
            src_ptr+=data->img_bpp/8;
        }
    }
}

uint32_t del_char(){
    if(cursor + SCREEN_START>0xb8000){
        cursor-=2;
    }
    unsigned char *pointer = (unsigned char*)(SCREEN_START+cursor);
    *pointer=0;
    return cursor;
}