extern uint8_t read_port(uint16_t port);
extern void write_port(uint16_t port, uint8_t data);
extern void disable_interrupts();
extern void enable_interrupts();
#include<stdint.h>

void timer_init(void){
	write_port(0x21 , 0xFE);
}

void set_pit_count(unsigned count) {
	// Disable interrupts
	disable_interrupts();
 
	// Set low byte
    write_port(0x43,0b0000100);
	write_port(0x40,count&0xFF);		// Low byte
	write_port(0x40,(count&0xFF00)>>8);	// High byte
    enable_interrupts();
	return;
}
uint32_t read_pit_count(void) {
	uint32_t count = 0;
 
	// Disable interrupts
	disable_interrupts();
 
	// al = channel in bits 6 and 7, remaining bits clear
	//write_port(0x43,0b0000100);
 
	count = read_port(0x40);		// Low byte
	count |= read_port(0x40)<<8;		// High byte
    enable_interrupts();
	return count;
}

void timer_task(void){
    write_port(0x20, 0x20);
    uint8_t st[]="Timer Interrupt";
    print_text(st);
    write_port(0x21 , 0xFD);
}