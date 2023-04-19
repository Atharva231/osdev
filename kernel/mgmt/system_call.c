#define SYSCALL_BUFF_LEN 4
#include <stdint.h>
#include <stdbool.h>
static uint32_t syscall_buff[SYSCALL_BUFF_LEN];
static struct Process_Control_Block* pcb_head=0;
uint32_t* get_syscall_buff(){
	return syscall_buff;
}
struct Process_Control_Block* get_pcb_head(){
    return pcb_head;
}
void system_call_task(){
    uint8_t delim[]=",";
    uint32_t temp=0;
    bool temp_bool=false,f;
    uint16_t c=0,t=0;
    uint32_t f_size=0, ptr=0;
    uint8_t* file_names=(uint8_t*)syscall_buff[1];
    uint8_t buff[FILE_NAME_LEN];
    uint32_t* files;
    static struct Process_Control_Block* pcb_ptr=0;
    switch(syscall_buff[0]){
    case 1:
    	if(syscall_buff[1]==1)
    		set_print_status(true);
    	else if(syscall_buff[1]==0){
    		set_print_status(false);
    		set_key_buff((uint8_t*)syscall_buff[2]);
    	}
    	break;
    case 2:
    	temp=create_file((uint8_t*)syscall_buff[1]);
    	break;
    case 3:
    	read_file((uint8_t*)syscall_buff[1],(uint8_t*)syscall_buff[2]);
    	break;
    case 4:
    	temp_bool=update_file((uint8_t*)syscall_buff[1], (uint8_t*)syscall_buff[2],syscall_buff[3]);
    	if(temp_bool==true)
    		temp=1;
    	else
    		temp=0;
    	break;
    case 5:
    	delete_file((uint8_t*)syscall_buff[1]);
    	break;
    case 6:
    	temp=file_size((uint8_t*)syscall_buff[1]);
    	break;
    case 7:
    	temp=count_files();
    	break;
    case 8:
    	list_files((uint8_t*)syscall_buff[1]);
    	break;
    case 9:
    	temp_bool=rename_file((uint8_t*)syscall_buff[1],(uint8_t*)syscall_buff[2]);
    	if(temp_bool==true)
    		temp=1;
    	else
    		temp=0;
    	break;
    case 10:
    	temp_bool=create_dir((uint8_t*)syscall_buff[1]);
    	if(temp_bool==true)
    		temp=1;
    	else
    		temp=0;
    	break;
    case 11:
    	chg_dir((uint8_t*)syscall_buff[1]);
    	break;
    case 12:
    	dir_up();
    	break;
    case 13:
    	delete_dir((uint8_t*)syscall_buff[1]);
    	break;
    case 14:
    	reset_dir_ptr();
    	break;
    case 15:
    	temp=count_dirs();
    	break;
    case 16:
    	list_dirs((uint8_t*)syscall_buff[1]);
    	break;
    case 17:
    	temp_bool=rename_dir((uint8_t*)syscall_buff[1],(uint8_t*)syscall_buff[2]);
    	if(temp_bool==true)
    		temp=1;
    	else
    		temp=0;
    	break;
    case 18:
    	update_fat();
    	break;
    case 19:
    	temp=load_elf(syscall_buff[1]);
        syscall_buff[1]=alloc_pages(0x100000);
    	break;
    case 20:
        syscall_buff[3]=get_heap_size(syscall_buff[1], 0);
        syscall_buff[2]=alloc_pages(syscall_buff[3]+0x100000);
        temp=load_link_elf(syscall_buff[1], 0, syscall_buff[2]);
        syscall_buff[1]=(syscall_buff[2]+syscall_buff[3]+0x100000)-1;
        break;
    case 21:
        temp=link_elf(syscall_buff[1],syscall_buff[2]);
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
        syscall_buff[1]=(uint32_t)files;
        syscall_buff[3]=get_heap_size(syscall_buff[1], 0);
        syscall_buff[2]=alloc_pages(syscall_buff[3]+0x100000);
        temp=load_link_elf(syscall_buff[1], 0, syscall_buff[2]);
        syscall_buff[1]=(syscall_buff[2]+syscall_buff[3]+0x100000)-1;
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
        pcb_ptr->bss_start=syscall_buff[2];
        pcb_ptr->stack_start=syscall_buff[1];
        uint32_t* ptr=(uint32_t*)pcb_ptr->stack_start;
        *ptr=pcb_ptr->pid;
        pcb_ptr->esp=pcb_ptr->stack_start-4;
        pcb_ptr->entry_addr=temp;
        temp=(uint32_t)pcb_ptr;
        break;
    }
    syscall_buff[0]=temp;
}
