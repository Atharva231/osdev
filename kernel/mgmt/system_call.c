#define SYSLOCK_LEN 4
#define STACK_SIZE 0x10000
#define FILE_NAME_LEN 19
#define DIR_NAME_LEN 15
#define ENTRIES_PER_FILE 1
#include <stdint.h>
#include <stdbool.h>

uint32_t* cmd;
bool syslock[SYSLOCK_LEN];
struct Process_Control_Block* pcb_head=0;
bool buff_lock=false;
void syscall_init(){
    for(uint8_t i=0;i<SYSLOCK_LEN;i++){
        syslock[i]=false;
    }
    buff_lock=false;
}
void set_syscall_buff(uint32_t* buff){
    while(buff_lock);
    buff_lock=true;
    cmd=buff;
}
struct Process_Control_Block* get_pcb_head(){
    return pcb_head;
}
void system_call_task(){
    bool temp_bool=false,f;
    uint32_t* files;
    struct Process_Control_Block* pcb_ptr=0;
    if(cmd[0]==1){
        while(syslock[0]);
        syslock[0]=true;
        uint32_t* scr_sysbuff=(uint32_t*)cmd[1];
        buff_lock=false;
        if(scr_sysbuff[0]==1)
            set_print_status(true);
        else if(scr_sysbuff[0]==0){
            set_print_status(false);
            set_key_buff((uint8_t*)scr_sysbuff[1]);
        }
        syslock[0]=false;
    }
    else if(cmd[0]>1 && cmd[0]<19){
        while(syslock[1]);
        syslock[1]=true;
        uint32_t* fs_sysbuff=(uint32_t*)cmd[1];
        buff_lock=false;
        switch (cmd[0])
        {
        case 2:
            fs_sysbuff[0]=create_file((uint8_t*)fs_sysbuff[0]);
            break;
        case 3:
            read_file((uint8_t*)fs_sysbuff[0],(uint8_t*)fs_sysbuff[1]);
            break;
        case 4:
            temp_bool=update_file((uint8_t*)fs_sysbuff[0], (uint8_t*)fs_sysbuff[1],fs_sysbuff[2]);
            if(temp_bool==true)
                fs_sysbuff[0]=1;
            else
                fs_sysbuff[0]=0;
            break;
        case 5:
            delete_file((uint8_t*)fs_sysbuff[0]);
            break;
        case 6:
            fs_sysbuff[0]=file_size((uint8_t*)fs_sysbuff[0]);
            break;
        case 7:
            fs_sysbuff[0]=count_pwd_files();
            break;
        case 8:
            list_files((uint8_t*)fs_sysbuff[0]);
            break;
        case 9:
            temp_bool=rename_file((uint8_t*)fs_sysbuff[0],(uint8_t*)fs_sysbuff[1]);
            if(temp_bool==true)
                fs_sysbuff[0]=1;
            else
                fs_sysbuff[0]=0;
            break;
        case 10:
            temp_bool=create_dir((uint8_t*)fs_sysbuff[0]);
            if(temp_bool==true)
                fs_sysbuff[0]=1;
            else
                fs_sysbuff[0]=0;
            break;
        case 11:
            chg_dir((uint8_t*)fs_sysbuff[0]);
            break;
        case 12:
            dir_up();
            break;
        case 13:
            delete_dir((uint8_t*)fs_sysbuff[0]);
            break;
        case 14:
            reset_dir_ptr();
            break;
        case 15:
            fs_sysbuff[0]=count_pwd_dirs();
            break;
        case 16:
            list_dirs((uint8_t*)fs_sysbuff[0]);
            break;
        case 17:
            temp_bool=rename_dir((uint8_t*)fs_sysbuff[0],(uint8_t*)fs_sysbuff[1]);
            if(temp_bool==true)
                fs_sysbuff[0]=1;
            else
                fs_sysbuff[0]=0;
            break;
        case 18:
            update_fat();
            break;
        
        default:
            break;
        }
        syslock[1];
    }
    else if(cmd[0]>=19 && cmd[0]<24){
        while(syslock[2]);
        syslock[2]=true;
        uint32_t* pm_sysbuff=(uint32_t*)cmd[1];
        buff_lock=false;
        uint16_t c=0,t=0;
        uint32_t f_size=0, ptr=0, ad;
        uint8_t* file_names=(uint8_t*)pm_sysbuff[0];
        uint8_t buff[FILE_NAME_LEN];
        switch(cmd[0]){
        case 19:
            pm_sysbuff[0]=load_elf(pm_sysbuff[0]);
            pm_sysbuff[0]=alloc_pages(STACK_SIZE);
            break;
        case 20:
            files=(uint32_t*)pm_sysbuff[0];
            pm_sysbuff[2]=get_heap_size(pm_sysbuff[0], 0);
            pm_sysbuff[1]=alloc_pages(pm_sysbuff[2]+STACK_SIZE);
            ad=load_link_elf(pm_sysbuff[0], 0, pm_sysbuff[1]);
            pm_sysbuff[0]=(pm_sysbuff[1]+pm_sysbuff[2]+STACK_SIZE)-1;
            if(pcb_head==0){
                pcb_head=create_pcb_list();
                pcb_ptr=pcb_head;
            }
            else{
                pcb_ptr=add_pcb_node(pcb_head);
            }
            pcb_ptr->pstat=1;
            pcb_ptr->text_start=files[0];
            pcb_ptr->text_size=f_size;
            pcb_ptr->bss_start=pm_sysbuff[1];
            pcb_ptr->stack_start=pm_sysbuff[0];
            uint32_t* ptr1=(uint32_t*)pcb_ptr->stack_start;
            *ptr1=pcb_ptr->pid;
            pcb_ptr->esp=pcb_ptr->stack_start-4;
            pcb_ptr->entry_addr=ad;
            pcb_ptr->apic_id=get_apic_id();
            pm_sysbuff[0]=(uint32_t)pcb_ptr;
            break;
        case 21:
            pm_sysbuff[0]=link_elf(pm_sysbuff[0],pm_sysbuff[1]);
            break;
        case 22:
            c=0;
            while(file_names[c]!=0){
                for(uint8_t i=0;i<FILE_NAME_LEN;i++){
                    buff[i]=0;
                }
                for(uint8_t i=0;file_names[c]!=' ';i++,c++){
                    buff[i]=file_names[c];
                }
                c+=1;
                f_size+=file_size(buff);
                t+=1;
            }
            files=(uint32_t*)mem_alloc(t*4);
            c=0;
            t=0;
            while(file_names[c]!=0){
                for(uint8_t i=0;i<FILE_NAME_LEN;i++){
                    buff[i]=0;
                }
                for(uint8_t i=0;file_names[c]!=' ';i++,c++){
                    buff[i]=file_names[c];
                }
                c+=1;
                if(t==0){
                    files[t]=alloc_pages(f_size);
                }
                else{
                    files[t]=ptr;
                }
                ptr=files[t]+file_size(buff);
                read_file(buff, (uint8_t*)files[t]);
                t+=1;
            }
            pm_sysbuff[0]=(uint32_t)files;
            pm_sysbuff[2]=get_heap_size(pm_sysbuff[0], 0);
            pm_sysbuff[1]=alloc_pages(pm_sysbuff[2]+STACK_SIZE);
            ad=load_link_elf(pm_sysbuff[0], 0, pm_sysbuff[1]);
            pm_sysbuff[0]=(pm_sysbuff[1]+pm_sysbuff[2]+STACK_SIZE)-1;
            if(pcb_head==0){
                pcb_head=create_pcb_list();
                pcb_ptr=pcb_head;
            }
            else{
                pcb_ptr=add_pcb_node(pcb_head);
            }
            pcb_ptr->pstat=1;
            pcb_ptr->text_start=files[0];
            pcb_ptr->text_size=f_size;
            pcb_ptr->bss_start=pm_sysbuff[2];
            pcb_ptr->stack_start=pm_sysbuff[2];
            uint32_t* ptr=(uint32_t*)pcb_ptr->stack_start;
            *ptr=pcb_ptr->pid;
            pcb_ptr->esp=pcb_ptr->stack_start-4;
            pcb_ptr->entry_addr=ad;
            pcb_ptr->apic_id=get_apic_id();
            pm_sysbuff[0]=(uint32_t)pcb_ptr;
            free_mem((uint32_t)files, t*4);
            break;
        
        case 23:
            pcb_ptr=get_pcb(pcb_head, pm_sysbuff[0]);
            if((uint32_t)pcb_ptr==1)
                break;
            unalloc_pages(pcb_ptr->text_start, pcb_ptr->text_size);
            unalloc_pages(pcb_ptr->bss_start, (pcb_ptr->stack_start - pcb_ptr->bss_start + 1));
            remove_pcb_node(pcb_head, pcb_ptr);
            break;
        
        default:
            break;
        }
        syslock[2]=false;
    }
    else if(cmd[0]>=24 && cmd[0]<26){
        while(syslock[3]);
        syslock[3]=true;
        uint32_t* rtc_sysbuff=(uint32_t*)cmd[1];
        buff_lock=false;
        switch (cmd[0])
        {
        case 24:
            read_time(rtc_sysbuff);
            break;

        case 25:
            read_date(rtc_sysbuff);
            break;
    
        default:
            break;
        }
        syslock[3]=false;
    }
    else if(cmd[0]>=26 && cmd[0]<28){
        while(syslock[4]);
        syslock[4]=true;
        uint32_t* mem_sysbuff=(uint32_t*)cmd[1];
        buff_lock=false;
        switch (cmd[0])
        {
        case 26:
            mem_sysbuff[0]=alloc_pages(mem_sysbuff[0]);
            break;
        
        case 27:
            unalloc_pages(mem_sysbuff[0], mem_sysbuff[1]);
            break;

        default:
            break;
        }
        syslock[4]=false;
    }
}
