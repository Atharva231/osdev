#define FMT_SIZE 256
#define MAT_SIZE 512
#include <stdint.h>

static uint32_t fmt[FMT_SIZE][2];
//static uint32_t mat[MAT_SIZE][2];
static uint32_t lowest_free_record_fmt;
//static uint32_t lowest_free_record_mat;
static uint32_t heap_start_addr;

void heap_init(uint32_t heap_start){
    heap_start_addr=heap_start;
    for(uint16_t i=0;i<FMT_SIZE;i++){
        fmt[i][0]=0;
        fmt[i][1]=0;
    }
/*    for(uint16_t i=0;i<MAT_SIZE;i++){
        mat[i][0]=0;
        mat[i][1]=0;
    }
    mat[0][0] = 0x00090000;     //stack start
    mat[0][1] = 0x0009FFFF;     //stack end
    mat[1][0] = 0x000A0000;     //video memory start
    mat[1][1] = 0x000BFFFF;     //video memory end*/
    fmt[0][0] = heap_start_addr;
    fmt[0][1] = 0x8FFFF;
    fmt[1][0] = 0xC0000;
    fmt[1][1] = 0xFFFDF;
    lowest_free_record_fmt = 2;
    //lowest_free_record_mat = 2;
}

uint32_t mem_alloc(uint32_t mem_size){
    uint32_t mem_start=0,t;
    uint16_t i;
    for(i=0;fmt[i][1]<(fmt[i][0]+mem_size-1)&&i<FMT_SIZE;i++){
    }
    if(i<FMT_SIZE){
        mem_start=fmt[i][0];
        fmt[i][0]+=mem_size;
	t=fmt[i][0];
        if(fmt[i][0]==fmt[i][1]){
            fmt[i][0]=0;
            fmt[i][1]=0;
            if(i<lowest_free_record_fmt){
                lowest_free_record_fmt=i;
            }
        }
/*        mat[lowest_free_record_mat][0]=mem_start;
        mat[lowest_free_record_mat][1]=t-1;
        while(mat[lowest_free_record_mat][0]!=0){
            lowest_free_record_mat+=1;
        }*/
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

uint32_t free_mem(uint32_t addr, uint32_t num_bytes){
    uint32_t resp=1;
    uint16_t i=0,f=MAT_SIZE,r=MAT_SIZE,j=0;
    /*while(mat[i][0]!=addr && i<MAT_SIZE){
        i++;
    }
    if(mat[i][0]!=addr){
        return;
    }*/
    while((f==MAT_SIZE || r==MAT_SIZE)&&j<FMT_SIZE){
        if(addr==(fmt[j][1]+1)){
            f=j;
        }
        else if((addr+num_bytes)==(fmt[j][0])){
            r=j;
        }
        j++;
    }
    if(f!=MAT_SIZE && r==MAT_SIZE){
        fmt[f][1]=(addr+num_bytes)-1;
    }
    else if(f==MAT_SIZE && r!=MAT_SIZE){
        fmt[r][0]=addr;
    }
    else if(f==MAT_SIZE && r==MAT_SIZE){
        uint16_t k,g;
        for(k=0;k<FMT_SIZE;k++){
            if((num_bytes-1) < (fmt[k][1]-fmt[k][0])){
                break;
            }
        }
        for(g=k+1;g<FMT_SIZE;g++){
            if(fmt[g][0]==0){
                break;
            }
        }
        for(uint8_t l=g;l>k;l--){
            fmt[l][0]=fmt[l-1][0];
            fmt[l][1]=fmt[l-1][1];
        }
        fmt[k][0]=addr;
        fmt[k][1]=addr+num_bytes-1;
        while(fmt[lowest_free_record_fmt][0]!=0){
            lowest_free_record_fmt+=1;
        }
    }
    else if(f!=MAT_SIZE && r!=MAT_SIZE){
        if(f<r){
            if(fmt[f][1]<fmt[r][1])
                fmt[f][1]=fmt[r][1];
            if(fmt[f][0]>fmt[r][0])
                fmt[f][0]=fmt[r][0];
            fmt[r][0]=0;
            fmt[r][1]=0;
            if(f<lowest_free_record_fmt){
                lowest_free_record_fmt=f;
            }
        }
        else{
            if(fmt[r][1]<fmt[f][1])
                fmt[r][1]=fmt[f][1];
            if(fmt[r][0]>fmt[f][0])
                fmt[r][0]=fmt[f][0];
            fmt[f][0]=0;
            fmt[f][1]=0;
            if(r<lowest_free_record_fmt){
                lowest_free_record_fmt=r;
            }
        }
    }
    /*if(i>0){
        mat[i][0] = 0;
        mat[i][1] = 0;
        if(lowest_free_record_mat>i){
            lowest_free_record_mat = i;
        }
        resp=2;
    }*/
	return fmt[0][0];
}
