#include<stdint.h>

void change_vector(uint8_t vector, uint32_t offset);
extern uint8_t port_byte_in(uint16_t port);
extern void port_byte_out(uint16_t port, uint8_t data);
extern void disable_interrupts();
extern void enable_interrupts();
void unmask_io_interrupt(uint32_t offset);
void mask_io_interrupt(uint32_t offset);

void set_pit_count(uint32_t freq, uint32_t mode) {
	if(freq<=18 || freq>1193182){
		return;
	}
	uint32_t count = 1193182/freq;
	// Disable interrupts
	disable_interrupts();
	// Set low byte
    port_byte_out(0x43, mode & 0xFFFF);
	port_byte_out(0x40,count&0xFF);		// Low byte
	port_byte_out(0x40,(count&0xFF00)>>8);	// High byte
    enable_interrupts();
	return;
}
uint32_t read_pit_count(void) {
	uint32_t count = 0;
 
	// Disable interrupts
	disable_interrupts();
 
	count = port_byte_in(0x40);		// Low byte
	count |= port_byte_in(0x40)<<8;		// High byte
    enable_interrupts();
	return count;
}

void pit_timer_init(void){
	//port_byte_out(0x21 , 0xFE);
	set_pit_count(0x1234DD, 0x30);
	asm("xor %eax, %eax");	//dummy instruction
	unmask_io_interrupt(0x04);
}
