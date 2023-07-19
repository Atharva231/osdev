#define FSFMT_SIZE 256
#define FSMAT_SIZE 512
#include <stdint.h>

uint32_t fsfmt[FSFMT_SIZE][2];
//static uint32_t mat[MAT_SIZE][2];
uint32_t lowest_free_record_fsfmt;
//static uint32_t lowest_free_record_mat;

uint32_t fs_mem_alloc(uint32_t mem_size){
    uint32_t mem_start=0;
    uint16_t i;
    for(i=0;fsfmt[i][1]<(fsfmt[i][0]+mem_size-1)&&i<FSFMT_SIZE;i++){
    }
    if(i<FSFMT_SIZE){
        mem_start=fsfmt[i][0];
        fsfmt[i][0]+=mem_size;
        if(fsfmt[i][0]==fsfmt[i][1]){
            fsfmt[i][0]=0;
            fsfmt[i][1]=0;
            if(i<lowest_free_record_fsfmt){
                lowest_free_record_fsfmt=i;
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

uint32_t fs_free_mem(uint32_t addr, uint32_t num_bytes){
    uint32_t resp=1;
    uint16_t i=0,f=FSMAT_SIZE,r=FSMAT_SIZE,j=0;
    /*while(mat[i][0]!=addr && i<MAT_SIZE){
        i++;
    }
    if(mat[i][0]!=addr){
        return;
    }*/
    while((f==FSMAT_SIZE || r==FSMAT_SIZE)&&j<FSFMT_SIZE){
        if(addr==(fsfmt[j][1]+1)){
            f=j;
        }
        else if((addr+num_bytes)==(fsfmt[j][0])){
            r=j;
        }
        j++;
    }
    if(f!=FSMAT_SIZE && r==FSMAT_SIZE){
        fsfmt[f][1]=(addr+num_bytes)-1;
    }
    else if(f==FSMAT_SIZE && r!=FSMAT_SIZE){
        fsfmt[r][0]=addr;
    }
    else if(f==FSMAT_SIZE && r==FSMAT_SIZE){
        uint16_t k,g;
        for(k=0;k<FSFMT_SIZE;k++){
            if((num_bytes-1) < (fsfmt[k][1]-fsfmt[k][0])){
                break;
            }
        }
        for(g=k+1;g<FSFMT_SIZE;g++){
            if(fsfmt[g][0]==0){
                break;
            }
        }
        for(uint8_t l=g;l>k;l--){
            fsfmt[l][0]=fsfmt[l-1][0];
            fsfmt[l][1]=fsfmt[l-1][1];
        }
        fsfmt[k][0]=addr;
        fsfmt[k][1]=addr+num_bytes-1;
        while(fsfmt[lowest_free_record_fsfmt][0]!=0){
            lowest_free_record_fsfmt+=1;
        }
    }
    else if(f!=FSMAT_SIZE && r!=FSMAT_SIZE){
        if(f<r){
            if(fsfmt[f][1]<fsfmt[r][1])
                fsfmt[f][1]=fsfmt[r][1];
            if(fsfmt[f][0]>fsfmt[r][0])
                fsfmt[f][0]=fsfmt[r][0];
            fsfmt[r][0]=0;
            fsfmt[r][1]=0;
            if(f<lowest_free_record_fsfmt){
                lowest_free_record_fsfmt=f;
            }
        }
        else{
            if(fsfmt[r][1]<fsfmt[f][1])
                fsfmt[r][1]=fsfmt[f][1];
            if(fsfmt[r][0]>fsfmt[f][0])
                fsfmt[r][0]=fsfmt[f][0];
            fsfmt[f][0]=0;
            fsfmt[f][1]=0;
            if(r<lowest_free_record_fsfmt){
                lowest_free_record_fsfmt=r;
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
	return fsfmt[0][0];
}
