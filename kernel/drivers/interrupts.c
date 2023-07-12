#define IDT_SIZE 256
#include<stdint.h>

extern void load_idt(uint32_t *addr);
extern void keyboard_handler();
extern void timer_handler();
extern void pause_handler();
extern void system_call_handler();
extern void page_fault_handler();
extern void ap_task_handler();
extern void net_intr_handler();
extern void cont_switch();
extern uint32_t get_idt();
extern void halt();
void general_protec_task(uint32_t error_code){
    print_text("General Protection Fault ");
    print_num(error_code);
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
    halt();
}

void div_error(){
	print_text("Division Error !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void invalid_opcode(){
	print_text("Invalid Opcode !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void nmi_intr(){
	print_text("NMI Interrupt !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void breakpoint(){
	print_text("Breakpoint !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void overflow_exc(){
	print_text("Overflow Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void debug_exc(){
	print_text("Debug Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void bre_exc(){
	print_text("Bound Range Exceeded Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void dev_not_avail_exc(){
	print_text("Device not Available Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void double_fault(uint32_t error_code){
	print_text("Double Fault Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void invalid_tss_exc(uint32_t error_code){
	print_text("Invalid TSS Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void seg_not_pres(uint32_t error_code){
	print_text("Segment Not Present Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void stack_seg_fault(uint32_t error_code){
	print_text("Stack Segment Fault !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void fpe(){
	print_text("Floating Point Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void align_check_exc(uint32_t error_code){
	print_text("Alignment Check Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void mach_check_exc(){
	print_text("Machine Check Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void simd_fpe(){
	print_text("SIMD Floating Point Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void virt_exc(){
	print_text("Virtualization Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void cpe(uint32_t error_code){
	print_text("Control Protection Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void hie(){
	print_text("Hypervisor Injection Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void vmm_ce(uint32_t error_code){
	print_text("VMM Communication Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}

void security_exc(uint32_t error_code){
	print_text("Security Exception !! ");
	uint32_t f;
	asm("mov 4(%%ebp), %0":"=r"(f));
	print_num_hex(f);
	halt();
}
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

	func_addr = (uint32_t)div_error;
    IDT_entry[0x00].offset_lowerbits = func_addr & 0xffff;
    IDT_entry[0x00].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT_entry[0x00].zero = 0;
    IDT_entry[0x00].type_attr = 0x8F; /* TRAP_GATE */
    IDT_entry[0x00].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)debug_exc;
	IDT_entry[0x01].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x01].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x01].zero = 0;
	IDT_entry[0x01].type_attr = 0x8F; /* TRAP_GATE */
	IDT_entry[0x01].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)nmi_intr;
	IDT_entry[0x02].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x02].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x02].zero = 0;
	IDT_entry[0x02].type_attr = 0x8F; /* TRAP_GATE */
	IDT_entry[0x02].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)breakpoint;
	IDT_entry[0x03].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x03].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x03].zero = 0;
	IDT_entry[0x03].type_attr = 0x8F; /* TRAP_GATE */
	IDT_entry[0x03].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)overflow_exc;
	IDT_entry[0x04].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x04].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x04].zero = 0;
	IDT_entry[0x04].type_attr = 0x8F; /* TRAP_GATE */
	IDT_entry[0x04].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)bre_exc;
	IDT_entry[0x05].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x05].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x05].zero = 0;
	IDT_entry[0x05].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x05].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)invalid_opcode;
    IDT_entry[0x06].offset_lowerbits = func_addr & 0xffff;
    IDT_entry[0x06].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT_entry[0x06].zero = 0;
    IDT_entry[0x06].type_attr = 0x8F; /* TRAP_GATE */
    IDT_entry[0x06].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)dev_not_avail_exc;
	IDT_entry[0x07].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x07].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x07].zero = 0;
	IDT_entry[0x07].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x07].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)double_fault;
	IDT_entry[0x08].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x08].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x08].zero = 0;
	IDT_entry[0x08].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x08].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)invalid_tss_exc;
	IDT_entry[0x0A].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x0A].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x0A].zero = 0;
	IDT_entry[0x0A].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x0A].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)seg_not_pres;
	IDT_entry[0x0B].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x0B].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x0B].zero = 0;
	IDT_entry[0x0B].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x0B].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)stack_seg_fault;
	IDT_entry[0x0C].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x0C].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x0C].zero = 0;
	IDT_entry[0x0C].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x0C].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)general_protec_task;
    IDT_entry[0x0D].offset_lowerbits = func_addr & 0xffff;
    IDT_entry[0x0D].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT_entry[0x0D].zero = 0;
    IDT_entry[0x0D].type_attr = 0x8F; /* TRAP_GATE */
    IDT_entry[0x0D].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)page_fault_handler;
	IDT_entry[0x0E].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x0E].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x0E].zero = 0;
	IDT_entry[0x0E].type_attr = 0x8F; /* TRAP_GATE */
	IDT_entry[0x0E].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)fpe;
	IDT_entry[0x10].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x10].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x10].zero = 0;
	IDT_entry[0x10].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x10].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)align_check_exc;
	IDT_entry[0x11].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x11].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x11].zero = 0;
	IDT_entry[0x11].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x11].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)mach_check_exc;
	IDT_entry[0x12].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x12].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x12].zero = 0;
	IDT_entry[0x12].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x12].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)simd_fpe;
	IDT_entry[0x13].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x13].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x13].zero = 0;
	IDT_entry[0x13].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x13].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)virt_exc;
	IDT_entry[0x14].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x14].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x14].zero = 0;
	IDT_entry[0x14].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x14].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)cpe;
	IDT_entry[0x15].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x15].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x15].zero = 0;
	IDT_entry[0x15].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x15].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)hie;
	IDT_entry[0x1C].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x1C].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x1C].zero = 0;
	IDT_entry[0x1C].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x1C].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)vmm_ce;
	IDT_entry[0x1D].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x1D].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x1D].zero = 0;
	IDT_entry[0x1D].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x1D].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)security_exc;
	IDT_entry[0x1E].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x1E].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x1E].zero = 0;
	IDT_entry[0x1E].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x1E].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)timer_handler;
	IDT_entry[0x20].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x20].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x20].zero = 0;
	IDT_entry[0x20].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x20].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	/* populate IDT entry of keyboard's interrupt */
	func_addr = (uint32_t)keyboard_handler;
	IDT_entry[0x21].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x21].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x21].zero = 0;
	IDT_entry[0x21].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x21].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)pause_handler;
	IDT_entry[0x39].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x39].zero = 0;
	IDT_entry[0x39].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x39].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)ap_task_handler;
	IDT_entry[0x3A].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x3A].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x3A].zero = 0;
	IDT_entry[0x3A].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x3A].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)cont_switch;
	IDT_entry[0x3B].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x3B].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x3B].zero = 0;
	IDT_entry[0x3B].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x3B].offset_higherbits = (func_addr & 0xffff0000) >> 16;

	func_addr = (uint32_t)system_call_handler;
	IDT_entry[0x80].offset_lowerbits = func_addr & 0xffff;
	IDT_entry[0x80].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT_entry[0x80].zero = 0;
	IDT_entry[0x80].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT_entry[0x80].offset_higherbits = (func_addr & 0xffff0000) >> 16;
	
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
	uint32_t idt_address = (uint32_t)IDT_entry ;
	uint32_t idt_ptr[2];
	idt_ptr[0] = (sizeof (struct InterruptDescriptor32) * IDT_SIZE) + ((idt_address & 0xffff) << 16) - 1;
	idt_ptr[1] = idt_address >> 16 ;
	load_idt(idt_ptr);
}

void set_interrupt(struct InterruptDescriptor32* intr, uint8_t offset){
	disable_interrupts();
	uint8_t* ptr = (uint8_t*)intr;
	uint8_t* idt_ptr1 = (uint8_t*)&((struct InterruptDescriptor32*)get_idt())[offset];
	for(uint8_t i=0;i<sizeof (struct InterruptDescriptor32);i++){
		idt_ptr1[i]=ptr[i];
	}
	enable_interrupts();
}

void get_idt_entry(struct InterruptDescriptor32* intr, uint8_t offset){
	disable_interrupts();
	uint8_t* ptr = (uint8_t*)intr;
	uint8_t* idt_ptr1 = (uint8_t*)&((struct InterruptDescriptor32*)get_idt())[offset];
	for(uint8_t i=0;i<sizeof (struct InterruptDescriptor32);i++){
		ptr[i]=idt_ptr1[i];
	}
	enable_interrupts();
}


