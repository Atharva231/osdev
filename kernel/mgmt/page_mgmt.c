#define FPT_SIZE 256
#define PAT_SIZE 512
#include <stdint.h>

static uint32_t fpt[FPT_SIZE][2];
static uint32_t lowest_free_record_fpt;

void page_alloc_init(uint32_t start){
    for(uint16_t i=0;i<FPT_SIZE;i++){
        fpt[i][0]=0;
        fpt[i][1]=0;
    }
    fpt[0][0] = start;
    fpt[0][1] = 0xFFFFFFFF;
    lowest_free_record_fpt = 1;
}

uint32_t page_alloc(uint32_t mem_size){
    uint32_t mem_start=0;
    uint16_t i;
    for(i=0;fpt[i][1]<(fpt[i][0]+mem_size-1)&&i<FPT_SIZE;i++){
    }
    if(i<FPT_SIZE){
        mem_start=fpt[i][0];
        fpt[i][0]+=mem_size;
        if(fpt[i][0]==fpt[i][1]){
            fpt[i][0]=0;
            fpt[i][1]=0;
            if(i<lowest_free_record_fpt){
                lowest_free_record_fpt=i;
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

uint32_t free_page(uint32_t addr, uint32_t num_bytes){
    uint32_t resp=1;
    uint16_t i=0,f=PAT_SIZE,r=PAT_SIZE,j=0;
    while((f==PAT_SIZE || r==PAT_SIZE)&&j<FPT_SIZE){
        if(addr==(fpt[j][1]+1)){
            f=j;
        }
        else if((addr+num_bytes)==(fpt[j][0])){
            r=j;
        }
        j++;
    }
    if(f!=PAT_SIZE && r==PAT_SIZE){
        fpt[f][1]=(addr+num_bytes)-1;
    }
    else if(f==PAT_SIZE && r!=PAT_SIZE){
        fpt[r][0]=addr;
    }
    else if(f==PAT_SIZE && r==PAT_SIZE){
        uint16_t k,g;
        for(k=0;k<FPT_SIZE;k++){
            if((num_bytes-1) < (fpt[k][1]-fpt[k][0])){
                break;
            }
        }
        for(g=k+1;g<FPT_SIZE;g++){
            if(fpt[g][0]==0){
                break;
            }
        }
        for(uint8_t l=g;l>k;l--){
            fpt[l][0]=fpt[l-1][0];
            fpt[l][1]=fpt[l-1][1];
        }
        fpt[k][0]=addr;
        fpt[k][1]=addr+num_bytes-1;
        while(fpt[lowest_free_record_fpt][0]!=0){
            lowest_free_record_fpt+=1;
        }
    }
    else if(f!=PAT_SIZE && r!=PAT_SIZE){
        if(f<r){
            if(fpt[f][1]<fpt[r][1])
                fpt[f][1]=fpt[r][1];
            if(fpt[f][0]>fpt[r][0])
                fpt[f][0]=fpt[r][0];
            fpt[r][0]=0;
            fpt[r][1]=0;
            if(f<lowest_free_record_fpt){
                lowest_free_record_fpt=f;
            }
        }
        else{
            if(fpt[r][1]<fpt[f][1])
                fpt[r][1]=fpt[f][1];
            if(fpt[r][0]>fpt[f][0])
                fpt[r][0]=fpt[f][0];
            fpt[f][0]=0;
            fpt[f][1]=0;
            if(r<lowest_free_record_fpt){
                lowest_free_record_fpt=r;
            }
        }
    }
	return fpt[0][0];
}
