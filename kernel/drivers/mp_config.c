#include <stdint.h>
struct MP_FP{
    uint8_t signature[4];
    uint32_t mp_cfg_addr;
    uint8_t length;
    uint8_t spec_rev;
    uint8_t checksum;
    uint8_t mp_feat_1;
    uint32_t mp_feat_5;
}* mp_ptr;

struct MP_CONFIG_HDR{
    uint8_t signature[4];
    uint16_t base_table_len;
    uint8_t spec_rev;
    uint8_t checksum;
    uint8_t oem_id[8];
    uint8_t prod_id[12];
    uint32_t oem_table_ptr;
    uint16_t oem_table_size;
    uint16_t entry_count;
    uint32_t local_apic_addr;
    uint16_t ext_table_len;
    uint8_t ext_table_checksum;
}* mp_config_tbl_hdr;

struct MP_ENTRY_0{
    uint8_t entry_type;
    uint8_t local_apic_id;
    uint8_t local_apic_ver;
    uint8_t cpu_flags;
    uint32_t cpu_sign;
    uint32_t feat_flags;
}* mp_entry_0;
struct MP_ENTRY_1{
    uint8_t entry_type;
    uint8_t bus_id;
    uint8_t bus_type[6];
}* mp_entry_1;
struct MP_ENTRY_2{
    uint8_t entry_type;
    uint8_t io_apic_id;
    uint8_t io_apic_ver;
    uint8_t io_apic_flags;
    uint32_t io_apic_addr;
}* mp_entry_2;
struct MP_ENTRY_3{
    uint8_t entry_type;
    uint8_t intr_type;
    uint16_t io_intr_flag;
    uint8_t src_bus_id;
    uint8_t src_bus_irq;
    uint8_t dest_ioapic_id;
    uint8_t dest_ioapic_intin;
}* mp_entry_3;
struct MP_ENTRY_4{
    uint8_t entry_type;
    uint8_t intr_type;
    uint16_t local_intr_flag;
    uint8_t src_bus_id;
    uint8_t src_bus_irq;
    uint8_t dest_lapic_id;
    uint8_t dest_lapic_lintin;
}* mp_entry_4;

uint32_t find_mp_ptr(){
    uint8_t* ptr=(uint32_t)0x0;
    uint32_t addr=0;
    for(uint32_t i=0;i<0xFFFFC; i++){
        if(ptr[i]=='_' && ptr[i+1]=='M' && ptr[i+2]=='P' && ptr[i+3]=='_'){
            addr = (((uint32_t)ptr)+i);
            break;
        }
    }
    mp_ptr=(struct MP_FP*)addr;
    if(addr==0){
        return 1;
    }
    mp_config_tbl_hdr=(struct MP_CONFIG_HDR*)mp_ptr->mp_cfg_addr;
    uint32_t cfg_tbl_strt=mp_ptr->mp_cfg_addr+sizeof(struct MP_CONFIG_HDR);
    for(uint32_t i=cfg_tbl_strt; i<mp_ptr->mp_cfg_addr+mp_config_tbl_hdr->base_table_len; i+=((*(uint8_t*)i)==0)?20:8){
        switch((*(uint8_t*)i)){
            case 0:
                mp_entry_0=(struct MP_ENTRY_0*)i;
                print_text("Entry 0: ");
                print_num_hex(mp_entry_0->local_apic_id);
                print_text(" ");
                print_num_hex(mp_entry_0->cpu_flags);
                print_text(" ");
                print_num_hex(mp_entry_0->cpu_sign);
                print_text(" ");
                print_num_hex(mp_entry_0->feat_flags);
                print_text(" ");
                break;
            case 1:
                mp_entry_1=(struct MP_ENTRY_1*)i;
                print_text("Entry 1: ");
                print_num_hex(mp_entry_1->bus_id);
                print_text(" ");
                print_text(mp_entry_1->bus_type);
                print_text(" ");
                break;
            case 2:
                mp_entry_2=(struct MP_ENTRY_2*)i;
                print_text("Entry 2: ");
                print_num_hex(mp_entry_2->io_apic_addr);
                print_text(" ");
                print_num_hex(mp_entry_2->io_apic_id);
                print_text(" ");
                break;
            /*case 3:
                mp_entry_3=(struct MP_ENTRY_3*)i;
                print_text("Entry 3: ");
                print_num_hex(mp_entry_3->src_bus_irq);
                print_text(" ");
                print_num_hex(mp_entry_3->src_bus_id);
                print_text(" ");
                print_num_hex(mp_entry_3->dest_ioapic_id);
                print_text(" ");
                print_num_hex(mp_entry_3->dest_ioapic_intin);
                print_text(" ");
                break;
            case 4:
                mp_entry_4=(struct MP_ENTRY_4*)i;
                print_text("Entry 4: ");
                print_num_hex(mp_entry_4->src_bus_irq);
                print_text(" ");
                print_num_hex(mp_entry_4->src_bus_id);
                print_text(" ");
                print_num_hex(mp_entry_4->dest_lapic_id);
                print_text(" ");
                print_num_hex(mp_entry_4->dest_lapic_lintin);
                print_text(" ");
                break;*/
            default:
                break;
        }
    }
    
}