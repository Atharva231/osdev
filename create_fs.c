#include "./os/os_init.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct file_list_element file;
void append_file(char* f){
    FILE* fptr=fopen(f, "ab");
    uint8_t c=0;
    for(uint8_t i=0; i<strlen(f);i++){
        if(f[i]=='/'){
            c++;
        }
    }
    for(uint8_t i=c,j=1; i<strlen(f);i++,j++){
        file.file_name[j]=f[i];
    }
    file.file_name[0]=',';
    
    fclose(fptr);
} 
void main(int argc, char *argv[]){
    
}