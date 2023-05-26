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
} *madt_mdata;
struct ENTRY_HEADER{
    uint8_t entry_type;
    uint8_t len;
} *madt_entry_hdr;
struct ENTRY_0{
    uint8_t acpi_id;
    uint8_t apic_id;
    uint32_t flag;
} *entry0;
struct ENTRY_1{
    uint8_t ioapic_id;
    uint8_t unused;
    uint32_t ioapic_addr;
    uint32_t gsib;
} *entry1;
struct ENTRY_2
{
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi;
    uint16_t flags;
} *entry2;
struct ENTRY_3
{
    uint8_t nmi_src;
    uint8_t unused;
    uint16_t flags;
    uint32_t gsi;
} *entry3;
struct ENTRY_4{
    uint8_t acpi_id;
    uint16_t flags;
    uint8_t lint;
} *entry4;
struct ENTRY_5
{
    uint16_t unused;
    uint64_t lapic_phys_addr;
} *entry5;

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
    entries=(madt->Length - sizeof(struct ACPISDTHeader))/4;
    madt_mdata=(struct MADT_MDATA*)(((uint32_t)ptr1) + 36);
    madt_entry_hdr=(struct ENTRY_HEADER*)(((uint32_t)ptr1) + 44);
    for(; (uint32_t)(madt_entry_hdr) < (uint32_t)(madt_entry_hdr)+madt->Length; madt_entry_hdr += madt_entry_hdr->len){
        switch (madt_entry_hdr->entry_type)
        {
        case 0:
            print_text("Entry 0: ");
            entry0=(struct ENTRY_0*)((uint32_t)madt_entry_hdr + 2);
            print_num_hex(entry0->acpi_id);
            print_text(" ");
            print_num_hex(entry0->apic_id);
            print_text(" ");
            print_num_hex(entry0->flag);
            print_text(" ");
            break;
        case 1:
            print_text("Entry 1: ");
            entry1=(struct ENTRY_1*)((uint32_t)madt_entry_hdr + 2);
            print_num_hex(entry1->ioapic_id);
            print_text(" ");
            print_num_hex(entry1->ioapic_addr);
            print_text(" ");
            print_num_hex(entry1->gsib);
            print_text(" ");
            break;
        case 2:
            print_text("Entry 2: ");
            entry2=(struct ENTRY_2*)((uint32_t)madt_entry_hdr + 2);
            print_num_hex(entry2->bus_src);
            print_text(" ");
            print_num_hex(entry2->irq_src);
            print_text(" ");
            print_num_hex(entry2->gsi);
            print_text(" ");
            print_num_hex(entry2->flags);
            print_text(" ");
            break;
        case 3:
            print_text("Entry 3: ");
            entry3=(struct ENTRY_3*)((uint32_t)madt_entry_hdr + 2);
            print_num_hex(entry3->nmi_src);
            print_text(" ");
            print_num_hex(entry3->gsi);
            print_text(" ");
            print_num_hex(entry3->flags);
            break;
        case 4:
            print_text("Entry 4: ");
            entry4=(struct ENTRY_4*)((uint32_t)madt_entry_hdr + 2);
            print_num_hex(entry4->acpi_id);
            print_text(" ");
            print_num_hex(entry4->lint);
            print_text(" ");
            print_num_hex(entry4->flags);
            break;
        default:
            break;
        }
    }
    return true;
}