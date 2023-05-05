#include <stdint.h>
#include <stdbool.h>
void rtc_init(){
    asm("cli");
    port_byte_out(0x70, 0x0B);
    port_byte_out(0x71, 0x06);
    asm("sti");
}
void read_time(uint32_t* rtc_buff){
    asm("cli");
    port_byte_out(0x70, 0x00);
    rtc_buff[0]=port_byte_in(0x71);
    port_byte_out(0x70, 0x02);
    rtc_buff[1]=port_byte_in(0x71);
    port_byte_out(0x70, 0x04);
    rtc_buff[2]=port_byte_in(0x71);
    asm("sti");
}
void read_date(uint32_t* rtc_buff){
    asm("cli");
    port_byte_out(0x70, 0x07);
    rtc_buff[2]=port_byte_in(0x71);
    port_byte_out(0x70, 0x08);
    rtc_buff[1]=port_byte_in(0x71);
    port_byte_out(0x70, 0x09);
    rtc_buff[0]=port_byte_in(0x71);
    asm("sti");
}
