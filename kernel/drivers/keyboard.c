#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0X64
#include <stdint.h>
#include <stdbool.h>
extern uint8_t read_port(uint16_t port);
extern void write_port(uint16_t port, uint8_t data);
static uint8_t prev_key=0;
static bool print_status=false;
uint8_t* key_buff;
void kb_init()
{
    /* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
    write_port(0x21 , 0xFD);
}
void set_print_status(bool f){
    print_status=f;
}
void set_key_buff(uint8_t* addr){
    key_buff=addr;
}
uint8_t keymap_small(uint8_t t){
    switch(t){
        case 1:
        return '`';
        case 2:
        return '1';
        case 3:
        return '2';
        case 4:
        return '3';
        case 5:
        return '4';
        case 6:
        return '5';
        case 7:
        return '6';
        case 8:
        return '7';
        case 9:
        return '8';
        case 10:
        return '9';
        case 11:
        return '0';
        case 12:
        return '-';
        case 13:
        return '=';
        case 16:
        return 'q';
        case 17:
        return 'w';
        case 18:
        return 'e';
        case 19:
        return 'r';
        case 20:
        return 't';
        case 21:
        return 'y';
        case 22:
        return 'u';
        case 23:
        return 'i';
        case 24:
        return 'o';
        case 25:
        return 'p';
        case 26:
        return '[';
        case 27:
        return ']';
        case 30:
        return 'a';
        case 31:
        return 's';
        case 32:
        return 'd';
        case 33:
        return 'f';
        case 34:
        return 'g';
        case 35:
        return 'h';
        case 36:
        return 'j';
        case 37:
        return 'k';
        case 38:
        return 'l';
        case 39:
        return ';';
        case 40:
        return '\'';
        case 43:
        return '\\';
        case 44:
        return 'z';
        case 45:
        return 'x';
        case 46:
        return 'c';
        case 47:
        return 'v';
        case 48:
        return 'b';
        case 49:
        return 'n';
        case 50:
        return 'm';
        case 51:
        return ',';
        case 52:
        return '.';
        case 53:
        return '/';
        case 57:
        return ' ';
        default:
        return '\0';
    }
}
uint8_t keymap_caps(uint8_t t){
    switch(t){
        case 1:
        return '~';
        case 2:
        return '!';
        case 3:
        return '@';
        case 4:
        return '#';
        case 5:
        return '$';
        case 6:
        return '%';
        case 7:
        return '^';
        case 8:
        return '&';
        case 9:
        return '*';
        case 10:
        return '(';
        case 11:
        return ')';
        case 12:
        return '_';
        case 13:
        return '+';
        case 16:
        return 'Q';
        case 17:
        return 'W';
        case 18:
        return 'E';
        case 19:
        return 'R';
        case 20:
        return 'T';
        case 21:
        return 'Y';
        case 22:
        return 'U';
        case 23:
        return 'I';
        case 24:
        return 'O';
        case 25:
        return 'P';
        case 26:
        return '{';
        case 27:
        return '}';
        case 30:
        return 'A';
        case 31:
        return 'S';
        case 32:
        return 'D';
        case 33:
        return 'F';
        case 34:
        return 'G';
        case 35:
        return 'H';
        case 36:
        return 'J';
        case 37:
        return 'K';
        case 38:
        return 'L';
        case 39:
        return ':';
        case 40:
        return '\"';
        case 43:
        return '|';
        case 44:
        return 'Z';
        case 45:
        return 'X';
        case 46:
        return 'C';
        case 47:
        return 'V';
        case 48:
        return 'B';
        case 49:
        return 'N';
        case 50:
        return 'M';
        case 51:
        return '<';
        case 52:
        return '>';
        case 53:
        return '?';
        case 57:
        return ' ';
        default:
        return '\0';
    }
}
uint32_t keyboard_get_print(){
    write_port(0x20, 0x20);
    uint32_t t,f=1;
    uint8_t str[] = ".";
    if(read_port(KEYBOARD_STATUS_PORT) & 0x1){
        t = read_port(KEYBOARD_DATA_PORT);
		if(t==14){
			del_char();
		}
        if(keymap_small(t)!='\0'){
            if(prev_key!=54){
                str[0] = keymap_small(t);
            }
            else{
                str[0] = keymap_caps(t);
            }
            if(prev_key==29 && t==45){
                f=2;
            }
            else if(prev_key==29 && t==19){
                f=3;
            }
    		if(print_status)
                print_text(str);
    		else{
    		    *key_buff=str[0];
    		    key_buff=(uint8_t*)(key_buff+1);
    		}
        }
        prev_key=t;
    }
    return f;
}
