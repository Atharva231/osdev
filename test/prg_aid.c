#include <stdint.h>
uint8_t msg2[]="Hello";
uint8_t buff[25];
uint32_t var;
void print(uint8_t* str, uint8_t* cursor){
	for(uint8_t i=0;i<str[i]!=0;i++,cursor+=2){
		*cursor=str[i];
	}
	for(uint8_t i=0;i<msg2[i]!=0;i++,cursor+=2){
		*cursor=msg2[i];
	}
	buff[0]=51;
	var=6;
}