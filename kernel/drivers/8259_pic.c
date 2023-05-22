#include <stdint.h>

void pic_8259_init(){
    /*     Ports
	*	 	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	port_byte_out(0x20 , 0x11);
	port_byte_out(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	port_byte_out(0x21 , 0x20);
	port_byte_out(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	port_byte_out(0x21 , 0x00);  
	port_byte_out(0xA1 , 0x00);  

	/* ICW4 - environment info */
	port_byte_out(0x21 , 0x01);
	port_byte_out(0xA1 , 0x01);
	/* Initialization finished */

}