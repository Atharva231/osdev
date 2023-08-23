#ifndef BMP_H
#define BMP_H
#include <stdint.h>
#include <stdbool.h>
#include "screen.h"
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
extern void display_bitmap(struct bmp_head* bmp_file, uint32_t* data, uint32_t framebuffer);
extern uint32_t calc_down_scale(uint32_t orig, uint32_t resize);
extern uint32_t calc_up_scale(uint32_t orig, uint32_t resize);

#endif