#include <stdint.h>
#include <stdbool.h>
struct Process_Control_Block{
	uint32_t pid;
	uint8_t pstat;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eip;
};