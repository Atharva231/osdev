#ifndef OS_INIT_H
#define OS_INIT_H

#define FILE_NAME_LEN 19
#define DIR_NAME_LEN 15
#define ENTRIES_PER_FILE 1
#define SHARED_MEM 0xDFF000
#define SELF_INTR_FUNC 0xDFF004
#include <stdint.h>
#include <stdbool.h>
struct __attribute__((__packed__)) Process_Control_Block{
	uint32_t pid;
	uint32_t pstat;
	uint32_t apic_id;
	uint32_t text_start;
	uint32_t text_size;
	uint32_t bss_start;
	uint32_t stack_start;
	uint32_t esp;
	uint32_t pflags;
	uint32_t entry_addr;
	struct Process_Control_Block* next;
};

struct __attribute__((__packed__)) file_list_element{
    uint8_t delim;
    uint8_t file_name[FILE_NAME_LEN];
    uint32_t file_addr[ENTRIES_PER_FILE][2];
    struct file_list_element* next;
};
struct __attribute__((__packed__)) dir_list_element{
    uint8_t delim;
    uint8_t dir_name[DIR_NAME_LEN];
    struct file_list_element* files_list;
    struct dir_list_element* dir_list;
    struct dir_list_element* up;
    struct dir_list_element* next;
};

struct InterruptDescriptor32 {
   	uint16_t offset_lowerbits;        // offset bits 0..15
   	uint16_t selector;        // a code segment selector in GDT or LDT
 	uint8_t  zero;            // unused, set to 0
   	uint8_t  type_attr; // gate type, dpl, and p fields
   	uint16_t offset_higherbits;        // offset bits 16..31
};

extern void syscall(uint16_t id, uint32_t* temp);
#endif