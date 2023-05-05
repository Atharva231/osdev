#include <stdint.h>
uint8_t msg1[]="Hello";
uint8_t buff[25];
uint32_t var;
void print(uint8_t* str, uint8_t* cursor){
	for(uint8_t i=0;i<str[i]!=0;i++,cursor+=2){
		*cursor=str[i];
	}
	for(uint8_t i=0;i<msg1[i]!=0;i++,cursor+=2){
		*cursor=msg1[i];
	}
	buff[0]=51;
	var=6;
}