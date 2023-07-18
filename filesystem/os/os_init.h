#ifndef OS_INIT_H
#define OS_INIT_H

#define FILE_NAME_LEN 19
#define DIR_NAME_LEN 15
#define ENTRIES_PER_FILE 1
#define SHARED_MEM 0xDFF000
#include <stdint.h>
#include <stdbool.h>
struct Process_Control_Block{
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

struct bmp_head{
    uint8_t bmp_sig[2];
    uint32_t file_size;
    uint32_t reserved;
    uint32_t data_offset;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    uint32_t horiz_res;
    uint32_t vert_res;
    uint32_t colors;
    uint32_t imp_colors;
};

extern void set_cursor(uint32_t cursor1);
extern uint32_t print_num_hex(uint32_t n);
extern uint32_t print_num(uint32_t n);
extern uint32_t print_text(uint8_t* str);
extern void clear_screen();
extern void set_pixel(uint16_t x, uint16_t y, uint8_t color);
extern void set_VGA_Frame(uint8_t* src);
extern uint32_t del_char();
extern void heap_init(uint32_t heap_start, uint32_t limit);
extern uint32_t mem_alloc(uint32_t mem_size);
extern uint32_t free_mem(uint32_t addr, uint32_t num_bytes);

#endif