#ifndef OS_INIT
#define OS_INIT

#include <stdint.h>
#include <stdbool.h>
#define FILE_NAME_LEN 19
#define DIR_NAME_LEN 15
#define ENTRIES_PER_FILE 1

struct file_list_element{
    uint8_t delim;
    uint8_t file_name[FILE_NAME_LEN];
    uint32_t file_addr[ENTRIES_PER_FILE][2];
    struct file_list_element* next;
};
struct dir_list_element{
    uint8_t delim;
    uint8_t dir_name[DIR_NAME_LEN];
    struct file_list_element* files_list;
    struct dir_list_element* dir_list;
    struct dir_list_element* up;
    struct dir_list_element* next;
};

struct Process_Control_Block{
	uint32_t pid;
	uint32_t pstat;
	uint8_t apic_id;
	uint32_t text_start;
	uint32_t text_size;
	uint32_t bss_start;
	uint32_t stack_start;
	uint32_t esp;
	uint32_t pflags;
	uint32_t entry_addr;
	struct Process_Control_Block* next;
};


#endif