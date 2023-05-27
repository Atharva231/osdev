void jumper();
void start(){
    jumper();
}
#include "../drivers/ata_pio.c"
#include <stdint.h>
extern void jump(uint32_t addr);
extern void print_char(uint32_t cursor, uint32_t chr);
extern void halt();
void jumper(){
    uint8_t* kernel = (uint8_t*)0x100000;
    uint16_t c=0;
    read_sectors((uint16_t*)kernel, 0x4, 0x5F);
    /*for(uint16_t i=0;i<512;i++,c+=2){
        print_char(0xb8000+c, kernel[i]);
    }
    halt();*/
    jump((uint32_t)&kernel[0x0]);
}
