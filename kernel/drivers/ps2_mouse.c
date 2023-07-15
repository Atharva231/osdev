#include <stdint.h>
#include <stdbool.h>
uint8_t* mouse_data_buff;
bool mouse_send_command(uint8_t mouse_cmd){
    while(port_byte_in(0x64)&2);
    port_byte_out(0x64, 0xD4);
    while(port_byte_in(0x64)&2);
    port_byte_out(0x60, mouse_cmd);
    while((port_byte_in(0x64)&1)==0);
    if(port_byte_in(0x60)==0xFA){
        print_text("Read successful ");
        return true;
    }
        print_text("Read not successful ");
    return false;
}

void mouse_task(){
    uint8_t i=0, data;
    while((port_byte_in(0x64)&1)!=0){
        data=port_byte_in(0x60);
        mouse_data_buff[i++]=data;
        print_num_hex(data);
    }
	print_text(" ");
}

void disable_mouse(){
    mask_io_interrupt(0x18);
}

void mouse_init(uint8_t* data_buff){
    mouse_data_buff=data_buff;
    port_byte_out(0xA1 , 0xFA);
    while(port_byte_in(0x64)&2);
    port_byte_out(0x64, 0xA8);
    mouse_send_command(0xFF);
    while((port_byte_in(0x64)&1)!=0){
        print_num_hex(port_byte_in(0x60));
    }
    mouse_send_command(0xF6);    
    mouse_send_command(0xF4);

    while(port_byte_in(0x64)&2);
    port_byte_out(0x64, 0x20);
    uint8_t stat=port_byte_in(0x60)|2;
    while(port_byte_in(0x64)&2);
    port_byte_out(0x64, 0x60);
    while(port_byte_in(0x64)&2);
    port_byte_out(0x60, stat);
}