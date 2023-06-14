#include <stdint.h>
#include "page_mgmt.c"
#include "physmem_mgmt.c"
#define FPHT_SIZE 256
extern void loadPageDirectory(uint32_t* addr);
extern void enablePaging();
extern void enable_PSE();
extern uint32_t read_cr2();

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t page_table[1024*1024] __attribute__((aligned(4096)));
static uint32_t page_location[1024*1024] __attribute__((aligned(4096)));
static uint32_t physmem_limit;
static uint8_t ovf=0;
static uint32_t kernel_space=0xE00;
static uint32_t* physmem_table;

void refresh_tlb(){
    loadPageDirectory(page_directory);
}
void save_page(uint32_t physaddr){
    uint32_t virtualaddr=0;
    uint32_t *pt;
    for(uint32_t i=kernel_space;i<0xFFFFF;i++){
        pt=(uint32_t*)(page_directory[i>>10] & 0xFFFFF000);
        if((pt[i&0x3FF]&0xFFFFF000)==physaddr){
            virtualaddr=i;
            break;
        }
    }
    uint32_t pdindex = (uint32_t)virtualaddr >> 10;
    uint32_t ptindex = (uint32_t)virtualaddr & 0x3FF;
    pt = (uint32_t*) (page_directory[pdindex] & 0xFFFFF000);
    uint32_t pl=((pdindex)*(ptindex));
    uint32_t t=sector_alloc(8);
    write_sectors(t, 8, (uint16_t*)physaddr);
    pt[ptindex]&=0xFFE;
    page_location[pl]=t;
}
void load_page(uint32_t virtualaddr){
    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x3FF;
    uint32_t *pt = (uint32_t*) (page_directory[pdindex] & 0xFFFFF000);
    uint32_t pl=((pdindex)*(ptindex)), physaddr=pt[ptindex]&0xFFFFF000;
    uint32_t t=page_location[pl];
    read_sectors((uint16_t*)physaddr, t, 8);
    free_sector(t, 8);
    pt[ptindex]|=0x1;
    page_location[pl]=0;
}
uint8_t map_page(uint32_t virtualaddr, uint32_t physaddr, uint32_t flags) {
    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x3FF;
    uint32_t *pt = (uint32_t*) (page_directory[pdindex] & 0xFFFFF000);
    if(pt==0)
        return 0;
    pt[ptindex] = physaddr | (flags & 0xFFF);
    page_directory[pdindex] |= 1;
    return 2;
}
uint32_t unmap_page(uint32_t virtualaddr){
    uint32_t pdindex = (uint32_t)virtualaddr >> 22;
    uint32_t ptindex = (uint32_t)virtualaddr >> 12 & 0x03FF;
    uint32_t *pt = (uint32_t*) (page_directory[pdindex] & 0xFFFFF000);
    if(pt==0)
        return 0;
    uint32_t temp=(pt[ptindex]&0xFFFFF000)/0x1000;
    pt[ptindex]=0;
    return temp*0x1000;
}
void vmm_init(uint32_t limit){
    physmem_limit=limit;
    uint32_t size=(0xFFFFF/limit)*FPHT_SIZE*2;
    if(size==0){
            size=FPHT_SIZE*2;
    }
    physmem_table=(uint32_t*)mem_alloc(size);
    page_alloc_init(kernel_space);
    physmem_alloc_init(kernel_space);
    for(uint16_t i = 0; i < 1024; i++)
    {
        page_directory[i] = ((uint32_t)&page_table[i*1024]) | 2;
    }
    for(uint16_t i=0; i<kernel_space; i++)
    {
        map_page((i * 0x1000), (i * 0x1000), 0x3);
    }
    for(uint32_t i=0xBF000; i<0xFFFFF; i++)
    {
        map_page((i * 0x1000), (i * 0x1000), 0x3);
    }
    loadPageDirectory(page_directory);
    enablePaging();
    enable_PSE();
}

void ap_vmm_init(){
    loadPageDirectory(page_directory);
    enablePaging();
    enable_PSE();
}

uint32_t alloc_pages(uint32_t size){
    if(size%0x1000!=0){
        size=size/0x1000+1;
    }
    else{
        size/=0x1000;
    }
    uint32_t page=0, virtualaddr, physaddr;
    /*for(uint32_t i=0;i<size;i++){
        virtualaddr=page_alloc(1)*0x1000;
        physaddr=physmem_alloc(1)*0x1000;
        if(physaddr==0){
            ovf+=1;
            physmem_alloc_init(kernel_space, physmem_limit, (uint32_t)&physmem_table[ovf*FPHT_SIZE*2]);
        }
        if(ovf>0){
            save_page(physaddr);
        }
        if(i==0){
            page=virtualaddr;
        }
        map_page(virtualaddr, physaddr, 0x3);
    }*/
    virtualaddr=page_alloc(size)*0x1000;
    physaddr=physmem_alloc(size)*0x1000;
    for(uint32_t i=0;i<size;i++){
        map_page((virtualaddr + i*0x1000), (physaddr + i*0x1000), 0x3);
    }
    return virtualaddr;
}

void unalloc_pages(uint32_t virtualaddr, uint32_t size){
    if(size%0x1000!=0){
        size=size/0x1000+1;
    }
    else{
        size/=0x1000;
    }
    uint32_t physaddr=0, temp;
    for(uint32_t i=0;i<size;i++){
        physaddr=unmap_page(virtualaddr + i*0x1000);
        free_page(((virtualaddr + i*0x1000)/0x1000), 1);
        temp=free_physmem((physaddr/0x1000), 1);
        /*if(ovf>0 && temp==kernel_space){
            ovf-=1;
            physmem_alloc_init(kernel_space, physmem_limit, (uint32_t)&physmem_table[ovf*FPHT_SIZE*2]);
        }*/
    }
}
void page_fault_task(uint32_t error_code){
    uint8_t space[]=" ";
    uint8_t msg[] = "Page Fault ";
    print_text(msg);
    print_num(error_code&0XF);
    print_text(space);
    uint32_t page=read_cr2();
    print_num_hex(page);
    page=page>>12;
    page*=0x1000;
    if(ovf>0){
        load_page(page);
    }
    /*else{
        map_page(page, physmem_alloc(1)*0x1000, 0x3);
    }*/
    while(1){}
}
