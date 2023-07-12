#define SCREEN_START 0xb8000
#define SCREEN_END 0xb9000
#define IDT_SIZE 256
#define VID_BLUE 32
#define VID_RED 40
#define VID_GREEN 48
#include<stdint.h>

static uint32_t cursor;
static uint8_t* VGA = (unsigned char*) 0xA0000;
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

uint32_t print_num_hex(uint32_t n){
    print_text("0x");
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
void clear_screen(){
    uint8_t *pointer = (uint8_t*)SCREEN_START;
    for(int i=0;i<0x1000;i+=2){
        pointer[i]=0;
    }
    set_cursor(0);
}
void set_pixel(uint16_t x, uint16_t y, uint8_t color) {
    VGA=(uint8_t*)0xA0000;
  uint16_t offset;
  if(0 <= x && x < 320) {
    if(0 <= y && y < 200) {
      offset = 320*y + x;
      VGA[offset] = color;
    }
  }
}
void set_VGA_Frame(uint8_t* src){
    VGA=(uint8_t*)0xA0000;
    uint16_t src_ptr=0,offset;
    for(uint16_t x=0;x<320;x++){
        for(uint16_t y=0;y<200;y++,src_ptr++){
            offset = 320*y + x;
            VGA[offset] = src[src_ptr];
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
