#define SCREEN_START 0xb8000
#define SCREEN_END 0xb9000
#define IDT_SIZE 256
#define VBE_INFO_LOC 0xA200
#include <stdint.h>
#include <stdbool.h>
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

uint32_t cursor;
uint8_t* VGA;
uint32_t scratchpad;
extern void syscall(uint16_t id, uint32_t* temp);

void set_cursor(uint32_t cursor1){
    cursor=cursor1;
}

void scratchpad_init(){
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)VBE_INFO_LOC;
    uint32_t vmem_size=vbe_info->width*vbe_info->height*(vbe_info->bpp/8);
    int temp[5];
    temp[0]=vmem_size;
    syscall(26, temp);
    scratchpad=temp[0];
}

uint32_t get_scratchpad(){
    return scratchpad;
}

void set_vesa_frame(){
    struct vbe_mode_info* vbe_info=(struct vbe_mode_info*)VBE_INFO_LOC;
    uint32_t vmem_size=vbe_info->width*vbe_info->height*(vbe_info->bpp/8);
    uint8_t* src=(uint8_t*)scratchpad;
    uint8_t* dst=(uint8_t*)vbe_info->framebuffer;
    for(uint32_t i=0;i<vmem_size;i++){
        dst[i]=src[i];
    }
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

uint32_t del_char(){
    if(cursor + SCREEN_START>0xb8000){
        cursor-=2;
    }
    unsigned char *pointer = (unsigned char*)(SCREEN_START+cursor);
    *pointer=0;
    return cursor;
}

