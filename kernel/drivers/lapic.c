#include <stdint.h>
#include <cpuid.h>
#define LAPIC_ID_REG 0x20
#define LAPIC_VER_REG 0x30
#define TPR 0x80
#define APR 0x90
#define PPR 0xA0
#define EOI_REG 0xB0
#define RRD 0xC0
#define LDR 0xD0
#define DFR 0xE0
#define SIVR 0xF0
#define ISR 0x100
#define TMR 0x180
#define IRR 0x200
#define ERR_STAT_REG 0x280
#define LVT_CMCI 0x2F0
#define ICR 0x300
#define LVT_TR 0x320
#define LVT_TSR 0x330
#define LVT_PMCR 0x340
#define LVT_LINT0 0x350
#define LVT_LINT1 0x360
#define LVT_ER 0x370
#define INITIAL_COUNT_REG 0x380
#define CCR 0x390
#define DCR 0x3E0

uint32_t* lapic_addr=(uint32_t*)0xFEE00000;

extern void get_msr(uint32_t code, uint32_t* data);
extern void set_msr(uint32_t code, uint32_t* data);
uint32_t check_apic(void)
{
    uint32_t eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & (1 << 9);
}
uint32_t get_apic_id(){
    uint32_t t, edx;
    asm("mov $0x1, %eax");
    asm("cpuid");
    asm("mov %%ebx, %0":"=r"(t));
    asm("mov %%edx, %0":"=r"(edx));
    return t>>16;
}
void lapic_init(){

    /* mask interrupts */
	port_byte_out(0x21 , 0xff);
	port_byte_out(0xA1 , 0xff);

    /* enable lapic using msr */
    uint32_t msr[2];
    get_msr(0x1B, msr);
    msr[0] |= 0x900;
    set_msr(0x1B, msr);
    
    /* enable spurious interrupt in lapic */
	lapic_addr[SIVR]=0xFF;
    
	/* set IMCR */
	port_byte_out(0x22, 0x70);
	uint8_t imcr=port_byte_in(0x23) | 0x01;
	port_byte_out(0x23, imcr);

    uint32_t data[7];
    data[0]=0x21;
    data[1]=0x00;
    data[2]=0x00;
    data[3]=0x01;
    data[4]=0x00;
    data[5]=0x00;
    data[6]=0x00;
    set_ioapic_redtbl(0x02, data);
	
}