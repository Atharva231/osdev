#include <stdint.h>
#include <stdbool.h>
#include "file_list.c"

static uint32_t filesystem_addr;
static struct dir_list_element* root;
static struct dir_list_element* dir_temp;
static struct dir_list_element* sub_dir;
static struct file_list_element* file_temp;
static uint8_t buff[512];
static uint32_t temp_file_addr[ENTRIES_PER_FILE][2];
uint16_t count_dirs(){
    uint16_t c=0;
    for(sub_dir=dir_temp->dir_list;sub_dir!=0;sub_dir=sub_dir->next,c++){

    }
    return c;
}
uint16_t count_files(){
    uint16_t c=0;
    file_temp=dir_temp->files_list;
    while(file_temp!=0){
        file_temp=file_temp->next;
        c++;
    }
    return c;
}
void filesystem_init(uint32_t addr){
    filesystem_addr = addr/0x200;
    bool f_end = true;
    uint16_t ptr;
    uint8_t* ptr_buff;
    root = create_dir_list("root");
    dir_temp=root;
    file_temp=0;
    ptr=0;
    read_sectors((uint16_t*)buff, filesystem_addr, 0x1);
    while(f_end){
        if(ptr>=512){
            ptr=0;
            read_sectors((uint16_t*)buff, filesystem_addr, 0x1);
            filesystem_addr+=1;
        }
        switch (buff[ptr])
        {
        case ' ':
        case ',':
        case '|':
            if(dir_temp->files_list==0){
                file_temp=create_file_list("temp", (uint8_t*)temp_file_addr);
                ptr_buff=(uint8_t*)file_temp;
                for(uint16_t i=0;i<sizeof(struct file_list_element);i++){
                    ptr_buff[i]=buff[ptr+i];
                }
                file_temp->next=0;
                dir_temp->files_list=file_temp;
        		for(uint8_t i=0;i<ENTRIES_PER_FILE;i++){
        		    if(file_temp->file_addr[i][1]!=0)
        		       sector_alloc(file_temp->file_addr[i][1]);
        		}
            }
            else{
                append_file_node("temp", (uint8_t*)temp_file_addr, dir_temp->files_list);
                file_temp=dir_temp->files_list;
                while(file_temp->next!=0){
                    file_temp=file_temp->next;
                }
                ptr_buff=(uint8_t*)file_temp;
                for(uint16_t i=0;i<sizeof(struct file_list_element);i++){
                    ptr_buff[i]=buff[ptr+i];
                }
                for(uint8_t i=0;i<ENTRIES_PER_FILE;i++){
                    if(file_temp->file_addr[i][1]!=0)
                       sector_alloc(file_temp->file_addr[i][1]);
                }
                file_temp->next=0;
            }
            if(buff[ptr]==','){
                file_temp=0;
            }
            else if(buff[ptr]=='|'){
                file_temp=0;
                dir_temp=dir_temp->dir_list;
            }
            ptr+=sizeof(struct file_list_element);
            break;

        case '.':
        case ';':
        case '/':
            if(dir_temp->dir_list==0){
                sub_dir=create_dir_list("temp");
                ptr_buff=(uint8_t*)sub_dir;
                for(uint16_t i=0;i<sizeof(struct dir_list_element);i++){
                    ptr_buff[i]=buff[ptr+i];
                }
                dir_temp->dir_list=sub_dir;
            }
            else{
                append_dir_node("temp", dir_temp->dir_list);
                sub_dir=dir_temp->dir_list;
                while(sub_dir->next!=0){
                    sub_dir=sub_dir->next;
                }
                ptr_buff=(uint8_t*)sub_dir;
                for(uint16_t i=0;i<sizeof(struct dir_list_element);i++){
                    ptr_buff[i]=buff[ptr+i];
                }
            }
            sub_dir->next=0;
            sub_dir->dir_list=0;
            sub_dir->up=dir_temp;
            if(buff[ptr]==';'){
                dir_temp=dir_temp->next;
            }
            else if(buff[ptr]=='/'){
                dir_temp=dir_temp->dir_list;
            }
            ptr+=sizeof(struct dir_list_element);
            break;

        case '!':
            f_end=false;
            break;

        default:
            break;
        }
    }
    dir_temp=root;
}
void update_fat(){
    struct dir_list_element* dir_ptr = (struct dir_list_element*)&root;
    uint8_t* file_ptr;
    uint16_t ptr=0;
    bool f=true;
    while(f){
    for(file_temp=dir_ptr->files_list;file_temp!=0;file_temp=file_temp->next){
	file_ptr=(uint8_t*)file_temp;
	for(uint16_t i=0;i<sizeof(struct file_list_element);i++,ptr++)
	    buff[ptr]=file_ptr[i];
	if(ptr>=512){
	    write_sectors(filesystem_addr, 0x1, (uint16_t*)buff);
	    for(uint16_t j=0;j<512;j++)
		buff[j]=0;
	    ptr=0;
	    filesystem_addr+=1;
        }
    }
    for(sub_dir=dir_ptr->dir_list;sub_dir!=0;sub_dir=sub_dir->next){
    	file_ptr=(uint8_t*)sub_dir;
	for(uint16_t i=0;i<sizeof(struct dir_list_element);i++,ptr++)
	    buff[ptr]=file_ptr[i];
        if(ptr>=512){
            write_sectors(filesystem_addr, 0x1, (uint16_t*)buff);
            for(uint16_t j=0;j<512;j++)
                buff[j]=0;
            ptr=0;
            filesystem_addr+=1;
        }
    }
    if(dir_ptr->next==0){
	if(dir_ptr->dir_list==0){
	    f=false;
	    buff[ptr]='!';
	    write_sectors(filesystem_addr, 0x1, (uint16_t*)buff);
	}
	else
	    dir_ptr=dir_ptr->dir_list;
    }
    else
	dir_ptr=dir_ptr->next;
    }
}
struct dir_list_element* pwd(){
    return dir_temp;
}
void list_files(uint8_t* buff){
    file_temp=dir_temp->files_list;
    uint16_t c=0;
    while(file_temp!=0){
        for(uint8_t i=0;file_temp->file_name[i]!=0;i++,c++){
            buff[c]=file_temp->file_name[i];
        }
        buff[c]=',';
        c+=1;
        file_temp=file_temp->next;
    }
    c-=1;
    buff[c]=0;
}
void dir_up(){
    if(dir_temp!=root){
        dir_temp=dir_temp->up;
    }
}
void reset_dir_ptr(){
    dir_temp=root;
}
struct file_list_element* search_file(uint8_t* file){
    uint8_t c,c1;
    for(file_temp=dir_temp->files_list; file_temp!=0; file_temp=file_temp->next){
        c=0;
        c1=0;
        for(uint8_t i=0;i<FILE_NAME_LEN && file[i]!=0;i++){
            c++;
        }
        for(uint8_t i=0;i<FILE_NAME_LEN && file_temp->file_name[i]!=0;i++){
            c1++;
        }
        if(c==c1){
            c=0;
            for(uint8_t i=0;i<c1;i++){
                if(file_temp->file_name[i]==file[i])
                    c++;
            }
            if(c==c1){
                return file_temp;
            }
        }
    }
    return (struct file_list_element*)1;
}
void refresh_file_addr(uint32_t addr[][2]){
    for(uint8_t i=0;i<ENTRIES_PER_FILE;i++){
        addr[i][0]=0;
        addr[i][1]=0;
    }
}
bool append_addr(uint32_t addr[][2], uint32_t arg1, uint32_t arg2){
    uint8_t i=0;
    while(i<ENTRIES_PER_FILE){
        if(addr[i][0]==0){
            addr[i][0]=arg1;
            addr[i][1]=arg2;
            return true;
        }
    }
    return false;
}
bool create_file(uint8_t* file){
    if((uint32_t)search_file(file)!=1){
        return false;
    }
    refresh_file_addr(temp_file_addr);
    if(dir_temp->files_list==0){
        file_temp=create_file_list(file, (uint8_t*)temp_file_addr);
        /*if(!append_addr(file_temp->file_addr, sector_alloc(1), 1)){
            return false;
        }*/
        dir_temp->files_list=file_temp;
    }
    else{
        append_file_node(file, (uint8_t*)temp_file_addr, dir_temp->files_list);
    }
    if(count_dirs()==0){
	for(file_temp=dir_temp->files_list;file_temp->next!=0;file_temp=file_temp->next){}
	file_temp->delim='|';
    }
    return true;
}

bool update_file(uint8_t* f_name, uint8_t* src_addr, uint32_t bytes){
    struct file_list_element* file = search_file(f_name);
    uint32_t sectors_len=bytes/512;
    if(bytes%512!=0){
        sectors_len+=1;
    }
    refresh_file_addr(file->file_addr);
    refresh_file_addr(temp_file_addr);
    temp_file_addr[0][0]=sector_alloc(sectors_len);
    if(temp_file_addr[0][0]==0){
        return false;
    }
    temp_file_addr[0][1]=sectors_len;
    write_sectors(temp_file_addr[0][0], sectors_len, (uint16_t*) src_addr);
    for(uint8_t i=0;i<ENTRIES_PER_FILE;i++){
    	if(file->file_addr[i][0]!=0)
    	   free_sector(file->file_addr[i][0], file->file_addr[i][1]);
    	file->file_addr[i][0]=temp_file_addr[i][0];
    	file->file_addr[i][1]=temp_file_addr[i][1];
    }
    return true;
}
uint32_t file_size(uint8_t* f_name){
    struct file_list_element* file = search_file(f_name);
    uint32_t s=0;
    for(uint8_t i=0;i<ENTRIES_PER_FILE;i++)
	   s+=file->file_addr[i][1];
    return s;
}
void read_file(uint8_t* f_name, uint8_t* dst_addr){
    struct file_list_element* file = search_file(f_name);
    uint32_t c=0,lba, f_size;
    for(uint8_t i=0;i<ENTRIES_PER_FILE;i++,c+=(file->file_addr[i][1]*200)){
        lba=file->file_addr[i][0]/0x200;
        if(file->file_addr[i][1]%0x200!=0)
            f_size=(file->file_addr[i][1]/0x200)+1;
        else
            f_size=file->file_addr[i][1]/0x200;
	   read_sectors((uint16_t*)&dst_addr[c], lba, f_size);
    }
}
struct dir_list_element* search_dir(uint8_t* dirName){
    uint8_t c,c1;
    for(sub_dir=dir_temp->dir_list; sub_dir!=0; sub_dir=sub_dir->next){
        c=0;
        c1=0;
        for(uint8_t i=0;i<DIR_NAME_LEN && dirName[i]!=0;i++){
            c++;
        }
        for(uint8_t i=0;i<DIR_NAME_LEN && sub_dir->dir_name[i]!=0;i++){
            c1++;
        }
        if(c==c1){
            c=0;
            for(uint8_t i=0;i<c1;i++){
                if(sub_dir->dir_name[i]==dirName[i])
                    c++;
            }
            if(c==c1){
                return sub_dir;
            }
        }
    }
    return (struct dir_list_element*)1;
}

bool chg_dir(uint8_t* dirName){
    sub_dir=search_dir(dirName);
    if((uint32_t)sub_dir==1){
        dir_temp=sub_dir;
        return true;
    }
    return false;
}

void delete_file(uint8_t* file_name){
    struct file_list_element* file=search_file(file_name);
    for(uint8_t i=0;i<ENTRIES_PER_FILE;i++){
	if(file->file_addr[i][1]==0)
	    continue;
	free_sector(file->file_addr[i][0], file->file_addr[i][1]);
    }
    file_temp=dir_temp->files_list;
    uint32_t c=0;
    while(file_temp!=file){
	file_temp=file_temp->next;
	c+=1;
    }
    dir_temp->files_list=remove_file_node(c, dir_temp->files_list);
}
bool create_dir(uint8_t* dir_name){
    if((uint32_t)search_dir(dir_name)!=1){
	return false;
    }
    if(dir_temp->dir_list==0){
	dir_temp->dir_list=create_dir_list(dir_name);
    }
    else{
	append_dir_node(dir_name, dir_temp->dir_list);
    }
    if(dir_temp->next==0){
	for(sub_dir=dir_temp->dir_list;sub_dir->next!=0;sub_dir=sub_dir->next){}
	sub_dir->delim='/';
    }
    return true;
}
void list_dirs(uint8_t* buff){
    uint16_t c=0;
    for(sub_dir=dir_temp->dir_list;sub_dir!=0;sub_dir=sub_dir->next){
        for(uint8_t i=0;sub_dir->dir_name[i]!=0;i++,c++){
            buff[c]=sub_dir->dir_name[i];
        }
        buff[c]=',';
        c+=1;
    }
    c-=1;
    buff[c]=0;
}
void delete_dir(uint8_t* dir_name){
    if(!chg_dir(dir_name))
        return;
    sub_dir=dir_temp;
    uint8_t space[]={' ','~'};
    for(file_temp=sub_dir->files_list;file_temp!=0;file_temp=file_temp->next){
        delete_file(file_temp->file_name);
    }
    uint16_t c=count_dirs(),c1;
    if(c!=0){
	for(sub_dir=dir_temp->dir_list;sub_dir!=0;sub_dir=sub_dir->next){
            delete_dir(sub_dir->dir_name);
        }
    }
    sub_dir=dir_temp;
    dir_up();
    struct dir_list_element* sub_dir1;
    uint32_t p=0;
    for(sub_dir1=dir_temp->dir_list;sub_dir1!=0;sub_dir1=sub_dir1->next,p++){
	    c=0;
        c1=0;
        for(uint8_t i=0;i<DIR_NAME_LEN && sub_dir->dir_name[i]!=0;i++){
            c++;
        }
        for(uint8_t i=0;i<DIR_NAME_LEN && sub_dir1->dir_name[i]!=0;i++){
            c1++;
        }
        if(c==c1){
            c=0;
            for(uint8_t i=0;i<c1;i++){
                if(sub_dir->dir_name[i]==sub_dir1->dir_name[i])
                    c++;
            }
            if(c==c1){
                dir_temp->dir_list=remove_dir_node(p, dir_temp->dir_list);
                return;
            }
        }
    }
}
bool rename_file(uint8_t* src_name, uint8_t* dest_name){
    file_temp=search_file(src_name);
    if(file_temp==(struct file_list_element*)1)
	    return false;
    for(uint8_t i=0;;i++){
        if(dest_name[i]==0){
            file_temp->file_name[i]=0;
            break;
        }
	    file_temp->file_name[i]=dest_name[i];
    }
    return true;
}
bool rename_dir(uint8_t* src_name, uint8_t* dest_name){
    sub_dir=search_dir(src_name);
    if(sub_dir==(struct dir_list_element*)1)
        return false;
    for(uint8_t i=0;;i++){
        if(dest_name[i]==0){
            sub_dir->dir_name[i]=0;
            break;
        }
	    sub_dir->dir_name[i]=dest_name[i];
    }
    return true;
}
