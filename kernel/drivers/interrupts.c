#define IDT_SIZE 256
#include<stdint.h>

extern void load_idt(uint32_t *addr);
extern void keyboard_handler();
extern void timer_handler();
extern void pause_handler();
extern void system_call_handler();
extern void page_fault_handler();
extern void ap_task_handler();
extern void halt();
void general_protec_task(uint32_t error_code);

struct InterruptDescriptor32 {
   	uint16_t offset_lowerbits;        // offset bits 0..15
   	uint16_t selector;        // a code segment selector in GDT or LDT
 	uint8_t  zero;            // unused, set to 0
   	uint8_t  type_attr; // gate type, dpl, and p fields
   	uint16_t offset_higherbits;        // offset bits 16..31
};
struct InterruptDescriptor32 IDT_entry[IDT_SIZE];
void idt_init(void){
   	uint32_t func_addr;
	uint32_t idt_address;
	uint32_t idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	func_addr = (uint32_t)keyboard_handler;
	IDT_entry[0x21].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x21].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x21].zero = 0;
	IDT_entry[0x21].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x21].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)timer_handler;
	IDT_entry[0x20].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x20].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x20].zero = 0;
	IDT_entry[0x20].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x20].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)pause_handler;
	IDT_entry[0x39].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x39].zero = 0;
	IDT_entry[0x39].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x39].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)system_call_handler;
	IDT_entry[0x80].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x80].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x80].zero = 0;
	IDT_entry[0x80].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x80].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)page_fault_handler;
	IDT_entry[0x0E].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x0E].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x0E].zero = 0;
	IDT_entry[0x0E].type_attr = 0x8F; /* TRAP_GATE */
	IDT_entry[0x0E].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)general_protec_task;
    IDT_entry[0x0D].offset_lowerbits = func_addr & 0xffff;
    IDT_entry[0x0D].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT_entry[0x0D].zero = 0;
    IDT_entry[0x0D].type_attr = 0x8F; /* TRAP_GATE */
    IDT_entry[0x0D].offset_higherbits = (func_addr & 0xffff0000) >> 16;
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

	/* fill the IDT descriptor */
	idt_address = (uint32_t)IDT_entry ;
	idt_ptr[0] = (sizeof (struct InterruptDescriptor32) * IDT_SIZE) + ((idt_address & 0xffff) << 16) - 1;
	idt_ptr[1] = idt_address >> 16 ;
	load_idt(idt_ptr);
}

void ap_idt_init(){
	struct InterruptDescriptor32* ap_idt=(struct InterruptDescriptor32*)mem_alloc(sizeof(struct InterruptDescriptor32)*IDT_SIZE);
	uint32_t func_addr = (uint32_t)ap_task_handler;
	ap_idt[0x20].offset_lowerbits = func_addr & 0xffff;
	ap_idt[0x20].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	ap_idt[0x20].zero = 0;
	ap_idt[0x20].type_attr = 0x8E; /* INTERRUPT_GATE */
	ap_idt[0x20].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)pause_handler;
	ap_idt[0x39].offset_lowerbits = func_addr & 0xffff;
	ap_idt[0x39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	ap_idt[0x39].zero = 0;
	ap_idt[0x39].type_attr = 0x8E; /* INTERRUPT_GATE */
	ap_idt[0x39].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)system_call_handler;
	ap_idt[0x80].offset_lowerbits = func_addr & 0xffff;
	ap_idt[0x80].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	ap_idt[0x80].zero = 0;
	ap_idt[0x80].type_attr = 0x8E; /* INTERRUPT_GATE */
	ap_idt[0x80].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)page_fault_handler;
	ap_idt[0x0E].offset_lowerbits = func_addr & 0xffff;
	ap_idt[0x0E].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	ap_idt[0x0E].zero = 0;
	ap_idt[0x0E].type_attr = 0x8F; /* TRAP_GATE */
	ap_idt[0x0E].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)general_protec_task;
    ap_idt[0x0D].offset_lowerbits = func_addr & 0xffff;
    ap_idt[0x0D].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    ap_idt[0x0D].zero = 0;
    ap_idt[0x0D].type_attr = 0x8F; /* TRAP_GATE */
    ap_idt[0x0D].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	uint32_t idt_address = (uint32_t)ap_idt ;
	uint32_t idt_ptr[2];
	idt_ptr[0] = (sizeof (struct InterruptDescriptor32) * IDT_SIZE) + ((idt_address & 0xffff) << 16) - 1;
	idt_ptr[1] = idt_address >> 16 ;
	load_idt(idt_ptr);
}

void general_protec_task(uint32_t error_code){
    uint8_t msg[]="General Protection Fault ";
    print_text(msg);
    print_num(error_code);
    halt();
}
