#include <stdint.h>
#include "page_allocation.c"
extern void loadPageDirectory(uint32_t* addr);
extern void enablePaging();
extern void enable_PSE();
extern uint32_t read_cr2();

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t page_table[1024*1024] __attribute__((aligned(4096)));
static uint32_t page_f;
void refresh_tlb(){
    loadPageDirectory(page_directory);
}
uint8_t map_page(uint32_t virtualaddr, uint32_t flags) {
    // Make sure that both addresses are page-aligned.

    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;

    uint32_t *pt = (uint32_t*) (page_directory[pdindex] & 0xFFFFF000);
    if(pt==0) return 0;
    uint32_t physaddr1=page_alloc(1)*0x1000;
    if(physaddr1==0)
	return 0;
    pt[ptindex] = ((uint32_t)physaddr1) | (flags & 0xFFF) | 0x01; // Present
    page_directory[pdindex] |= 1;
    return 1;
}

uint32_t get_table_flags(uint32_t virtualaddr){
    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;
    uint32_t *pt = (uint32_t*) (page_directory[pdindex] & 0xFFFFF000);
    return (pt[ptindex] & 0xFFF);
}

void set_table_flags(uint32_t virtualaddr, uint32_t flags){
    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;
    uint32_t *pt = (uint32_t*) (page_directory[pdindex] & 0xFFFFF000);
    pt[ptindex]|=(flags & 0xFFF);
}

void vmm_init(){
    page_alloc_init();
    page_f=0;
    for(uint16_t i = 0; i < 1024; i++)
    {
        page_directory[i] = ((uint32_t)&page_table[i*1024]) | 2;
    }
    page_table[0]=3;
    uint16_t pages = 2304;
    for(uint16_t i = 1; i < pages; i++)
    {
        map_page((i * 0x1000), 2);
    }
    loadPageDirectory(page_directory);
    enablePaging();
    enable_PSE();
}

void page_fault_task(uint32_t error_code){
    uint8_t space[]=" ";
    uint8_t msg[] = "Page Fault ";
    print_text(msg);
    print_num(error_code&0XF);
    print_text(space);
    uint32_t page=read_cr2();
    print_num(page);
    page=page>>12;
    map_page(page*0x1000, 2);
}
