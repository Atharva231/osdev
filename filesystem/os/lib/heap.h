#ifndef HEAP_H
#define HEAP_H
#define FMT_SIZE 256
#define MAT_SIZE 512
#include <stdint.h>

extern void heap_init(uint32_t heap_start, uint32_t limit);
extern uint32_t mem_alloc(uint32_t mem_size);
extern uint32_t free_mem(uint32_t addr, uint32_t num_bytes);

#endif