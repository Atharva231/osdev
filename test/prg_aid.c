#include <stdint.h>
uint8_t msg1[]="Hello";
void print(uint8_t* str, uint8_t* cursor){
	for(uint8_t i=0;i<str[i]!=0;i++,cursor+=2){
		*cursor=str[i];
	}
	for(uint8_t i=0;i<msg1[i]!=0;i++,cursor+=2){
		*cursor=msg1[i];
	}
}