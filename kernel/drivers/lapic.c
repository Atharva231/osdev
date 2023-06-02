#include <stdint.h>
#include <cpuid.h>
#include <stdbool.h>
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
#define ICR_LOW 0x300
#define ICR_HIGH 0x310
#define LVT_TR 0x320
#define LVT_TSR 0x330
#define LVT_PMCR 0x340
#define LVT_LINT0 0x350
#define LVT_LINT1 0x360
#define LVT_ER 0x370
#define INITIAL_COUNT_REG 0x380
#define CCR 0x390
#define DCR 0x3E0

#define INTR_VECTOR(data) (data & 0xFFFF)
#define DELMOD(data) ((data<<8) & 0x700)
#define DESTMOD(data) ((data<<11) & 0x800)
#define LEVEL(data) ((data<<14) & 0x4000)
#define TRIGGER_MODE(data) ((data<<15) & 0x8000)
#define DEST_SHORTHAND(data) ((data<<18) & 0xC0000)
#define DEST_FIELD(data) ((data<<24) & 0xFF000000)

uint32_t* lapic_addr=(uint32_t*)0xFEE00000;
uint32_t timer_calib=0;
bool sleep_flag;
extern void get_msr(uint32_t code, uint32_t* data);
extern void set_msr(uint32_t code, uint32_t* data);

uint32_t check_apic(void)
{
    uint32_t eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & (1 << 9);
}
uint32_t get_apic_id(){
    uint32_t ebx, edx, unused;
    __get_cpuid(1, &unused, &ebx, &unused, &edx);
    return ebx>>24;
}
void send_IPI(uint32_t lapic_id, uint32_t vector){
    lapic_addr[ICR_HIGH/4]=0xFF000000 & (lapic_id<<24);
    lapic_addr[ICR_LOW/4]=0x00004020;
}
void self_intr(uint32_t vector){
    uint32_t icr_lo=0;
    lapic_addr[ICR_HIGH/4]=0x00000000;
    icr_lo |= INTR_VECTOR(vector) | DELMOD(0x00) | DESTMOD(0x00);
    icr_lo |= LEVEL(0x01) | TRIGGER_MODE(0x00) | DEST_SHORTHAND(0x01);
    lapic_addr[ICR_LOW/4]=icr_lo;
}
uint8_t div_val(uint8_t div){
    switch (div)
    {
    case 1:
        return 0b1011;
    case 2:
        return 0b0000;
    case 4:
        return 0b0001;
    case 8:
        return 0b0010;
    case 16:
        return 0b0011;
    case 32:
        return 0b1000;
    case 64:
        return 0b1001;
    case 128:
        return 0b1010;
    default:
        break;
    }
}
void pause(){
    sleep_flag=false;
}
void set_apic_timer(uint32_t* data){
    uint32_t t = data[0] & 0xFF;
    t |= ((data[1]<<17) & 0x60000);
    lapic_addr[DCR/4] = (div_val(data[2] & 0xF) & 0xF);
    lapic_addr[LVT_TR/4] = t;
    lapic_addr[INITIAL_COUNT_REG/4] = data[3];
}
void timer_task(void){
    print_num_hex(timer_calib);
}
uint32_t calib_lapic_timer(){
    uint32_t data[]={0x20, 0x00, 1, 0xFFFFFFFF};
    uint32_t rtc[3];
    read_time(rtc);
    uint32_t ps=rtc[0];
    while(ps==rtc[0]){
        read_time(rtc);
    }
    read_time(rtc);
    ps=rtc[0];
    set_apic_timer(data);
    while(ps==rtc[0]){
        read_time(rtc);
    }
    timer_calib=*((uint32_t*)0xFEE00390);
    lapic_addr[LVT_TR/4] |= 0x10000;
    timer_calib = 0xFFFFFFFF-timer_calib;
    return timer_calib;
}

void sleep_s(uint32_t time){
    sleep_flag=true;
    uint32_t data[]={0x39, 0x00, 1, (timer_calib*time)};
    set_apic_timer(data);
    while (sleep_flag);
}
void sleep_ms(uint32_t time){
    sleep_flag=true;
    double t=timer_calib;
    t /=1000.0;
    uint32_t data[]={0x39, 0x00, 1, (t*time)};
    set_apic_timer(data);
    while (sleep_flag);
}
void sleep_us(uint32_t time){
    sleep_flag=true;
    double t=timer_calib;
    t /=1000000.0;
    uint32_t data[]={0x39, 0x00, 1, (t*time)};
    set_apic_timer(data);
    while (sleep_flag);
}
void lapic_init(){

    /* mask 8259 pic interrupts */
	port_byte_out(0x21 , 0xff);
	port_byte_out(0xA1 , 0xff);

    /* enable lapic using msr */
    uint32_t msr[2];
    get_msr(0x1B, msr);
    msr[0] |= 0x900;
    set_msr(0x1B, msr);
    
    /* enable spurious interrupt in lapic */
	*((uint32_t*)0xFEE000F0)=0x1FF;

    /* set keyboard interrupt */
    uint32_t data[7];
    data[0]=0x21;
    data[1]=0x00;
    data[2]=0x00;
    data[3]=0x01;
    data[4]=0x00;
    data[5]=0x00;
    data[6]=0x00;
    set_ioapic_redtbl(0x02, data);
	
    /* set and mask 8254 timer interrupt */
    data[0]=0x39;
    data[1]=0x00;
    data[2]=0x00;
    data[3]=0x01;
    data[4]=0x00;
    data[5]=0x01;
    data[6]=0x00;
    set_ioapic_redtbl(0x04, data);

    /* set rtc interrupt */
    data[0]=0x20;
    data[1]=0x00;
    data[2]=0x00;
    data[3]=0x01;
    data[4]=0x00;
    data[5]=0x01;
    data[6]=0x00;
    set_ioapic_redtbl(0x08, data);
}

void ap_lapic_init(){

    /* enable lapic using msr */
    uint32_t msr[2];
    get_msr(0x1B, msr);
    msr[0] |= 0x900;
    set_msr(0x1B, msr);   
    
    /* enable spurious interrupt in lapic */
	*((uint32_t*)0xFEE000F0)=0x1FF;
}