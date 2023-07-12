#include <stdint.h>
extern uint32_t print_num(uint32_t n);
extern uint32_t print_text(uint8_t* str);
extern void set_cursor(uint32_t cursor1);
extern void clear_screen();
uint8_t msg[]="screen test";
void main(){
    set_cursor(0);
    clear_screen();
    print_num(56789);
    print_text(msg);
}
