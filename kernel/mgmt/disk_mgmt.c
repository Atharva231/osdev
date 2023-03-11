#define SAT_SIZE 256
#define FST_SIZE 256
#include <stdint.h>

static uint32_t fst[FST_SIZE][2];
static uint32_t lowest_free_record_fst, disk_start_addr;

uint32_t disk_init(uint32_t addr, uint32_t limit){
    disk_start_addr = addr;
    /*read_sectors((uint16_t*)fst, addr, (FST_SIZE*4*2)/512);
    addr+=(FST_SIZE*4*2)/512;
    read_sectors((uint16_t*)sat, addr, (SAT_SIZE*4*2)/512);
    addr+=(SAT_SIZE*4*2)/512;*/
    fst[0][0]=disk_start_addr;
    fst[0][1]=limit;
    for(uint16_t i=0;i<FST_SIZE;i++){
        if(fst[i][0]==0){
            lowest_free_record_fst = i;
            break;
        }
    }
    return addr;
}

void update_disk_table(){
    uint32_t addr = disk_start_addr;
    write_sectors(addr, (FST_SIZE*4*2)/512, (uint16_t*)fst);
    addr+=(FST_SIZE*4*2)/512;
    /*write_sectors(addr, (SAT_SIZE*4*2)/512, (uint16_t*)sat);
    addr+=(SAT_SIZE*4*2)/512;*/
}

uint32_t sector_alloc(uint32_t sector_count){
    uint32_t sector_start=0, t;
    uint16_t i;

    for(i=0;fst[i][1]<(fst[i][0]+sector_count-1)&&i<FST_SIZE;i++){
    }
    if(i<FST_SIZE){
        sector_start=fst[i][0];
        fst[i][0]+=sector_count;
	t=fst[i][0];
        if(fst[i][0]==fst[i][1]){
            fst[i][0]=0;
            fst[i][1]=0;
            if(i<lowest_free_record_fst){
                lowest_free_record_fst=i;
            }
        }
    }
    return sector_start;
}

uint32_t free_sector(uint32_t addr, uint32_t num_sectors){
    uint32_t resp=1;
    uint16_t i=0,f=SAT_SIZE,r=SAT_SIZE,j=0;
    while((f==SAT_SIZE || r==SAT_SIZE)&&j<FST_SIZE){
        if(addr==(fst[j][1]+1)){
            f=j;
        }
        else if((addr+num_sectors)==(fst[j][0])){
            r=j;
        }
        j++;
    }
    if(f!=SAT_SIZE && r==SAT_SIZE){
        fst[f][1]=(addr+num_sectors-1);
    }
    else if(f==SAT_SIZE && r!=SAT_SIZE){
        fst[r][0]=addr;
    }
    else if(f==SAT_SIZE && r==SAT_SIZE){
        uint16_t k,g;
        for(k=0;k<FST_SIZE;k++){
            if((num_sectors-1) < (fst[k][1]-fst[k][0])){
                break;
            }
        }
        for(g=k+1;g<FST_SIZE;g++){
            if(fst[g][0]==0){
                break;
            }
        }
        for(uint16_t l=g;l>k;l--){
            fst[l][0]=fst[l-1][0];
            fst[l][1]=fst[l-1][1];
        }
        fst[k][0]=addr;
        fst[k][1]=addr+num_sectors-1;
        while(fst[lowest_free_record_fst][0]!=0){
            lowest_free_record_fst+=1;
        }
    }
    else if(f!=SAT_SIZE && r!=SAT_SIZE){
        if(f<r){
            if(fst[f][1]<fst[r][1])
                fst[f][1]=fst[r][1];
            if(fst[f][0]>fst[r][0])
                fst[f][0]=fst[r][0];
            fst[r][0]=0;
            fst[r][1]=0;
            if(f<lowest_free_record_fst){
                lowest_free_record_fst=f;
            }
        }
        else{
            if(fst[r][1]<fst[f][1])
                fst[r][1]=fst[f][1];
            if(fst[r][0]>fst[f][0])
                fst[r][0]=fst[f][0];
            fst[f][0]=0;
            fst[f][1]=0;
            if(r<lowest_free_record_fst){
                lowest_free_record_fst=r;
            }
        }
    }
    return fst[0][1];
}
