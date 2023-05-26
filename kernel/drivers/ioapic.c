#include <stdint.h>
#define IOAPICID 0x00
#define IOAPICVER 0x01
#define IOAPICARB 0x02
#define IOREDTBL 0x10
#define INTR_VECTOR(data) (data & 0xFFFF)
#define DELMOD(data) ((data<<8) & 0x700)
#define DESTMOD(data) ((data<<11) & 0x800)
#define INTPOL(data) ((data<<13) & 0x2000)
#define TRIGGER_MODE(data) ((data<<15) & 0x8000)
#define INTERRUPT_MASK(data) ((data<<16) & 0x10000)
#define DEST_FIELD(data) ((data<<24) & 0xFF000000)

uint32_t* ioregsel=(uint32_t*)0xFEC00000;
uint32_t* iowin=(uint32_t*)0xFEC00010;
uint32_t get_ioapic_id(){
    *ioregsel=IOAPICID;
    return *iowin;
}
uint32_t get_ioapic_ver(){
    *ioregsel=IOAPICVER;
    return *iowin;
}
uint32_t get_ioapic_arb(){
    *ioregsel=IOAPICARB;
    return *iowin;
}
uint32_t get_ioapic_redtbl(uint32_t offset){
    *ioregsel=IOREDTBL+offset;
    return *iowin;
}
void unmask_interrupt(uint32_t offset){
    *ioregsel=IOREDTBL+offset;
    *iowin &= 0xFFFEFFFF;
}
void mask_interrupt(uint32_t offset){
    *ioregsel=IOREDTBL+offset;
    *iowin |= INTERRUPT_MASK(0x01);
}
void change_io_vector(uint8_t vector, uint32_t offset){
    if(offset%2!=0){
        return;
    }
    *ioregsel=IOREDTBL+offset;
    *iowin &= 0xFFFFFF00;
    *iowin |= (vector & 0xFF);
}
void set_ioapic_redtbl(uint32_t offset, uint32_t *data){
    uint32_t redtbl_lo=0;
    uint32_t redtbl_hi=0;
    redtbl_lo |= INTR_VECTOR(data[0]) | DELMOD(data[1]) | DESTMOD(data[2]);
    redtbl_lo |= INTPOL(data[3]) | TRIGGER_MODE(data[4]) | INTERRUPT_MASK(data[5]);
    redtbl_hi = DEST_FIELD(data[6]);
    *ioregsel=IOREDTBL+offset;
    *iowin=redtbl_lo;
    *ioregsel=IOREDTBL+offset+1;
    *iowin=redtbl_hi;
}