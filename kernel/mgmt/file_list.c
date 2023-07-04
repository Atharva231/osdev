#include <stdint.h>
#include <stdbool.h>
#define FILE_NAME_LEN 19
#define DIR_NAME_LEN 15
#define ENTRIES_PER_FILE 1

struct file_list_element{
    uint8_t delim;
    uint8_t file_name[FILE_NAME_LEN];
    uint32_t file_addr[ENTRIES_PER_FILE][2];
    struct file_list_element* next;
};
struct dir_list_element{
    uint8_t delim;
    uint8_t dir_name[DIR_NAME_LEN];
    struct file_list_element* files_list;
    struct dir_list_element* dir_list;
    struct dir_list_element* up;
    struct dir_list_element* next;
};


void refresh_name(uint8_t* src, bool flag){
    uint8_t len=FILE_NAME_LEN;
    if(flag){
        len=DIR_NAME_LEN;
    }
    for(uint8_t i=0;i<len;i++){
        src[i] = 0;
    }
}

struct file_list_element* create_file_list(uint8_t* name, uint8_t* addr){
    struct file_list_element* head = (struct file_list_element*)mem_alloc(sizeof(struct file_list_element));
    if(head==0){
        return 0;
    }
    for(uint8_t i=0;i<FILE_NAME_LEN;i++)
        head->file_name[i]=name[i];
    uint8_t* ptr = (uint8_t*)head->file_addr;
    for(uint8_t i=0;i<(2*FILE_NAME_LEN*ENTRIES_PER_FILE);i++)
        ptr[i] = addr[i];
    head->next=0;
    head->delim = ',';
    return head;
}
struct dir_list_element* create_dir_list(uint8_t* name){
    struct dir_list_element* head = (struct dir_list_element*)mem_alloc(sizeof(struct dir_list_element));
    if(head==0){
        return 0;
    }
    for(uint8_t i=0;i<DIR_NAME_LEN;i++)
        head->dir_name[i]=name[i];
    head->files_list=0;
    head->dir_list=0;
    head->next=0;
    head->up=0;
    head->delim = '.';
    return head;
}

uint8_t append_file_node(uint8_t* name, uint8_t* addr,struct file_list_element* head){
    if(head==0)
    return 0;
    struct file_list_element* node = (struct file_list_element*)mem_alloc(sizeof(struct file_list_element));
    if(node==0){
        return 0;
    }
    struct file_list_element* temp=head;
    for(uint8_t i=0;i<FILE_NAME_LEN;i++)
        node->file_name[i]=name[i];
    uint8_t* ptr = (uint8_t*)node->file_addr;
    for(uint8_t i=0;i<(2*FILE_NAME_LEN*ENTRIES_PER_FILE);i++)
        ptr[i] = addr[i];
    node->next = 0;
    node->delim = ',';
    while(temp->next!=0){
        temp=temp->next;
    }
    temp->next=node;
    return 1;
}
uint8_t append_dir_node(uint8_t* name, struct dir_list_element* head){
    if(head==0)
    return 0;
    struct dir_list_element* node = (struct dir_list_element*)mem_alloc(sizeof(struct dir_list_element));
    if(node==0){
        return 0;
    }
    struct dir_list_element* temp=head;
    for(uint8_t i=0;i<DIR_NAME_LEN;i++)
        node->dir_name[i]=name[i];
    node->dir_list=0;
    node->files_list=0;
    node->next = 0;
    node->delim = '.';
    while(temp->next!=0){
        temp=temp->next;
    }
    temp->next=node;
    return 1;
}

struct file_list_element* remove_file_node(uint32_t node_num, struct file_list_element* head){
    if(head==0||head->next==0)
	return head;
    struct file_list_element* temp=head;
    if(node_num==0){
        head=head->next;
        free_mem((uint32_t)temp, sizeof(struct file_list_element));
        return head;
    }
    for(uint32_t i=1;i<node_num;i++){
        temp=temp->next;
        if(temp==0)
	    return head;
    }
    struct file_list_element* temp1 = temp->next;
    temp->delim=temp->next->delim;
    temp->next = temp->next->next;
    free_mem((uint32_t)temp1, sizeof(struct file_list_element));
    return head;
}
struct dir_list_element* remove_dir_node(uint32_t node_num, struct dir_list_element* head){
    if(head==0||head->next==0)
    return head;
    struct dir_list_element* temp=head;
    if(node_num==0){
        head=head->next;
        free_mem((uint32_t)temp, sizeof(struct dir_list_element));
        return head;
    }
    for(uint32_t i=1;i<node_num;i++){
        temp=temp->next;
        if(temp==0)
        return head;
    }
    struct dir_list_element* temp1 = temp->next;
    temp->delim=temp->next->delim;
    temp->next = temp->next->next;
    free_mem((uint32_t)temp1, sizeof(struct dir_list_element));
    return head;
}
