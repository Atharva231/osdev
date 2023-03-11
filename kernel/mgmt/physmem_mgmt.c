#define FPHT_SIZE 256
#define PHAT_SIZE 512
#include <stdint.h>

static uint32_t fpht[FPHT_SIZE][2];
static uint32_t lowest_free_record_fpht;

void physmem_alloc_init(uint32_t start, uint32_t limit){
    for(uint16_t i=0;i<FPHT_SIZE;i++){
        fpht[i][0]=0;
        fpht[i][1]=0;
    }
    fpht[0][0] = start;
    fpht[0][1] = limit;
    lowest_free_record_fpht = 1;
}

uint32_t physmem_alloc(uint32_t mem_size){
    uint32_t mem_start=0;
    uint16_t i;
    for(i=0;fpht[i][1]<(fpht[i][0]+mem_size-1)&&i<FPHT_SIZE;i++){
    }
    if(i<FPHT_SIZE){
        mem_start=fpht[i][0];
        fpht[i][0]+=mem_size;
        if(fpht[i][0]==fpht[i][1]){
            fpht[i][0]=0;
            fpht[i][1]=0;
            if(i<lowest_free_record_fpht){
                lowest_free_record_fpht=i;
            }
        }
    }
    if(mem_start==0){
        return 0;
    }
    uint8_t* ptr = (uint8_t*)mem_start;
    for(uint32_t i=0;i<mem_size;i++){
        ptr[i]=0;
    }
    return mem_start;
}

uint32_t free_physmem(uint32_t addr, uint32_t num_bytes){
    uint32_t resp=1;
    uint16_t i=0,f=PHAT_SIZE,r=PHAT_SIZE,j=0;
    while((f==PHAT_SIZE || r==PHAT_SIZE)&&j<FPHT_SIZE){
        if(addr==(fpht[j][1]+1)){
            f=j;
        }
        else if((addr+num_bytes)==(fpht[j][0])){
            r=j;
        }
        j++;
    }
    if(f!=PHAT_SIZE && r==PHAT_SIZE){
        fpht[f][1]=(addr+num_bytes)-1;
    }
    else if(f==PHAT_SIZE && r!=PHAT_SIZE){
        fpht[r][0]=addr;
    }
    else if(f==PHAT_SIZE && r==PHAT_SIZE){
        uint16_t k,g;
        for(k=0;k<FPHT_SIZE;k++){
            if((num_bytes-1) < (fpht[k][1]-fpht[k][0])){
                break;
            }
        }
        for(g=k+1;g<FPHT_SIZE;g++){
            if(fpht[g][0]==0){
                break;
            }
        }
        for(uint8_t l=g;l>k;l--){
            fpht[l][0]=fpht[l-1][0];
            fpht[l][1]=fpht[l-1][1];
        }
        fpht[k][0]=addr;
        fpht[k][1]=addr+num_bytes-1;
        while(fpht[lowest_free_record_fpht][0]!=0){
            lowest_free_record_fpht+=1;
        }
    }
    else if(f!=PHAT_SIZE && r!=PHAT_SIZE){
        if(f<r){
            if(fpht[f][1]<fpht[r][1])
                fpht[f][1]=fpht[r][1];
            if(fpht[f][0]>fpht[r][0])
                fpht[f][0]=fpht[r][0];
            fpht[r][0]=0;
            fpht[r][1]=0;
            if(f<lowest_free_record_fpht){
                lowest_free_record_fpht=f;
            }
        }
        else{
            if(fpht[r][1]<fpht[f][1])
                fpht[r][1]=fpht[f][1];
            if(fpht[r][0]>fpht[f][0])
                fpht[r][0]=fpht[f][0];
            fpht[f][0]=0;
            fpht[f][1]=0;
            if(r<lowest_free_record_fpht){
                lowest_free_record_fpht=r;
            }
        }
    }
	return fpht[0][0];
}
