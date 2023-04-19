#include <stdint.h>
uint8_t msg[]="This is prg_main() ";
uint8_t msg1[]="This is atharva ";
uint8_t buff[256];
uint8_t ano_buff[256];
extern void print(uint8_t* str, uint8_t* cursor);
void func(uint8_t* cursor){
    for(uint8_t i=0;msg1[i]!=0;i++,cursor+=2){
        *cursor=msg1[i];
    }
}
void main(){
    uint8_t* cursor=(uint8_t*)0xB8000;
    for(uint8_t i=0;msg[i]!=0;i++,cursor+=2){
	   *cursor=msg[i];
    }
    func(cursor);
//    print_text(msg);
    msg[0]='I';
    msg[1]=' ';
    msg[2]='a';
    msg[3]='m';
    print(msg, cursor);
    buff[1]=50;
    ano_buff[3]=51;
    while(1){}
}
