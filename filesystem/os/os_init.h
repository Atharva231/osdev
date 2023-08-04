#ifndef OS_INIT_H
#define OS_INIT_H

#define FILE_NAME_LEN 19
#define DIR_NAME_LEN 15
#define ENTRIES_PER_FILE 1
#define SHARED_MEM 0xDFF000
#define SELF_INTR_FUNC 0xDFF004
#define VBE_INFO_LOC 0xA200
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
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

struct __attribute__((__packed__)) bmp_head{
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

struct vbe_mode_info {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));


struct __attribute__ ((packed)) vesa_frame{
    uint32_t frame_buff;
    uint32_t image;
    uint32_t img_width;
    uint32_t img_height;
    uint16_t pitch;
    uint8_t img_bpp;
	uint8_t screen_bpp;
};

struct InterruptDescriptor32 {
   	uint16_t offset_lowerbits;        // offset bits 0..15
   	uint16_t selector;        // a code segment selector in GDT or LDT
 	uint8_t  zero;            // unused, set to 0
   	uint8_t  type_attr; // gate type, dpl, and p fields
   	uint16_t offset_higherbits;        // offset bits 16..31
};

extern void set_cursor(uint32_t cursor1);
extern uint32_t print_num_hex(uint32_t n);
extern uint32_t print_num(uint32_t n);
extern uint32_t print_text(uint8_t* str);
extern void clear_screen();
extern uint32_t del_char();
extern void set_vesa_frame(struct vesa_frame* data, uint32_t x_offset, uint32_t y_offset);
extern void heap_init(uint32_t heap_start, uint32_t limit);
extern uint32_t mem_alloc(uint32_t mem_size);
extern uint32_t free_mem(uint32_t addr, uint32_t num_bytes);
extern void syscall(uint16_t id, uint32_t* temp);
extern void display_bitmap(uint8_t* file, uint32_t x_offset, uint32_t y_offset);
#endif