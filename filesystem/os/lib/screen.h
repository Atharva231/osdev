#ifndef SCREEN_H
#define SCREEN_H
#define SCREEN_START 0xb8000
#define SCREEN_END 0xb9000
#define VBE_INFO_LOC 0xA200
#include <stdint.h>
#include <stdbool.h>
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
	uint8_t screen_bpp;
    uint32_t frame_buff;
    uint16_t pitch;
    uint32_t x_offset;
    uint32_t y_offset;
    uint32_t image;
    uint32_t img_width;
    uint32_t img_height;
    uint8_t img_bpp;
	uint32_t resize_width;
	uint32_t resize_height;
	int scale;
};
extern void set_cursor(uint32_t cursor1);
extern void scratchpad_init();
extern uint32_t get_scratchpad();
extern void set_vesa_frame();
extern void clear_scratchpad();
extern uint32_t print_color_text(uint8_t *str, uint8_t color);
extern uint32_t print_num_hex(uint32_t n);
extern uint32_t print_num(uint32_t n);
extern uint32_t print_text(uint8_t* str);
extern void clear_screen();
extern uint32_t del_char();
#endif