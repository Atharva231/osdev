#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC
#include <stdint.h>
extern uint32_t port_dword_in(uint32_t);
extern void port_dword_out(uint32_t, uint32_t);
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    port_dword_out(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((port_dword_in(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

void pciConfigWriteByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    port_dword_out(0xCF8, address);
    tmp = port_dword_in(0xCFC) & 0x00FFFFFF;
    tmp |= (data<<24);
    port_dword_out(0xCF8, address);
    port_dword_out(0xCFC, tmp);
}

uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function)
{
        uint32_t r0 =  pciConfigReadWord(bus,device,function,0);
        return r0;
}
uint16_t getDeviceID(uint16_t bus, uint16_t device, uint16_t function)
{
        uint32_t r0 =  pciConfigReadWord(bus,device,function,2);
        return r0;
}
uint16_t getClassID(uint16_t bus, uint16_t device, uint16_t function)
{
        uint32_t r0 =  pciConfigReadWord(bus,device,function,0xA);
        return (r0 & 0xFF00) >> 8;
}
uint16_t getSubClassID(uint16_t bus, uint16_t device, uint16_t function)
{
        uint32_t r0 =  pciConfigReadWord(bus,device,function,0xA);
        return (r0 & 0x00FF);
}
uint16_t getHeaderType(uint16_t bus, uint16_t device, uint16_t function)
{
        uint32_t r0 =  pciConfigReadWord(bus,device,function,0xE);
        return (r0 & 0x00FF);
}
uint32_t getBAR(uint16_t bus, uint16_t device, uint16_t function, uint8_t bar){
        uint8_t offset = bar*4 + 0x10;
        uint32_t r0 =  pciConfigReadWord(bus,device,function,offset);
        uint32_t r1 =  pciConfigReadWord(bus,device,function,offset+2);
        r1=r1<<16;
        r1&=0xFFFF0000;
        return (r0|r1);
}