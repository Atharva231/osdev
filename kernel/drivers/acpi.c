#include <stdint.h>
#include <stdbool.h>
struct RSDPDescriptor {
    uint8_t Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed));

struct ACPISDTHeader {
    uint8_t Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    uint8_t OEMID[6];
    uint8_t OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
};
struct MADT_MDATA{
    uint8_t* lapic_id;
    uint32_t flag;
};
struct ENTRY_HEADER{
    uint8_t entry_type;
    uint8_t len;
};
struct ENTRY_0{
    uint8_t acpi_id;
    uint8_t apic_id;
    uint32_t flag;
};
struct ENTRY_1{
    uint8_t ioapic_id;
    uint8_t unused;
    uint32_t ioapic_addr;
    uint32_t gsib;
};
struct ENTRY_2
{
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi;
    uint16_t flags;
};
struct ENTRY_3
{
    uint8_t nmi_src;
    uint8_t unused;
    uint16_t flags;
    uint32_t gsi;
};
struct ENTRY_4{
    uint8_t acpi_id;
    uint16_t flags;
    uint8_t lint;
};
struct ENTRY_5
{
    uint16_t unused;
    uint64_t lapic_phys_addr;
};

static struct RSDPDescriptor* rsdp;
static struct ACPISDTHeader* acpi_sdt;
uint32_t find_rsdp(){
    uint16_t* ebda=(uint16_t*)0x040E;
    uint8_t* ptr=(uint8_t*)(*ebda<<4);
    for(uint32_t i=0;i<0x603FC;i++){
        if(ptr[i]=='R' && ptr[i+1]=='S' && ptr[i+2]=='D'){
            if(ptr[i+3]==' ' && ptr[i+4]=='P' && ptr[i+5]=='T' && ptr[i+6]=='R' && ptr[i+7]==' ')
                return ((*ebda<<4)+i);
        }
    }
    return 1;
}

bool acpi_init(){
    uint32_t rsdp_addr=find_rsdp();
    if(rsdp_addr==1){
        return false;
    }
    rsdp=(struct RSDPDescriptor*)rsdp_addr;
    acpi_sdt=(struct ACPISDTHeader*)rsdp->RsdtAddress;
    uint32_t entries = (acpi_sdt->Length - sizeof(struct ACPISDTHeader)) / 4;
    uint8_t *ptr1;
    uint32_t *ptr2=(uint32_t*)(rsdp->RsdtAddress + sizeof(struct ACPISDTHeader));
    for(uint8_t i=0;i<entries;i++){
        ptr1=(uint8_t*)(ptr2[i]);
        if(ptr1[0]=='A' && ptr1[1]=='P' && ptr1[2]=='I' && ptr1[3]=='C'){
            break;
        }
        if(i==entries-1){
            return false;
        }
    }
    struct ACPISDTHeader* madt=(struct ACPISDTHeader*)((uint32_t)ptr1);
    ptr2=(uint32_t*)(((uint32_t)ptr1) + 44);
    entries=(madt->Length - sizeof(struct ACPISDTHeader))/4;
    print_text(madt->Signature);
    print_text(" ");
    print_num_hex(*ptr2);
    return true;
}