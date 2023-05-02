extern uint8_t port_byte_in(uint16_t port);
extern void port_byte_out(uint16_t port, uint8_t data);
extern void disable_interrupts();
extern void enable_interrupts();
#include<stdint.h>

void timer_init(void){
	port_byte_out(0x21 , 0xFE);
}

void set_pit_count(unsigned count) {
	// Disable interrupts
	disable_interrupts();
 
	// Set low byte
    port_byte_out(0x43,0b0000100);
	port_byte_out(0x40,count&0xFF);		// Low byte
	port_byte_out(0x40,(count&0xFF00)>>8);	// High byte
    enable_interrupts();
	return;
}
uint32_t read_pit_count(void) {
	uint32_t count = 0;
 
	// Disable interrupts
	disable_interrupts();
 
	// al = channel in bits 6 and 7, remaining bits clear
	//write_port(0x43,0b0000100);
 
	count = port_byte_in(0x40);		// Low byte
	count |= port_byte_in(0x40)<<8;		// High byte
    enable_interrupts();
	return count;
}

void timer_task(void){
    port_byte_out(0x20, 0x20);
    uint8_t st[]="Timer Interrupt";
    print_text(st);
    port_byte_out(0x21 , 0xFD);
}