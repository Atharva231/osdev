#include <stdint.h>
struct list_element{
    uint32_t data;
    struct list_element* next;
};
struct list_element* create_list(uint32_t d){
    struct list_element* head = (struct list_element*)mem_alloc(sizeof(struct list_element));
    head->data=d;
    head->next=0;
    return head;
}
uint8_t append_node(uint32_t d, struct list_element* head){
    if(head==0)
    return 0;
    struct list_element* node = (struct list_element*)mem_alloc(sizeof(struct list_element));
    node->data=d;
    node->next=0;
    struct list_element* temp=head;
    while(temp->next!=0){
        temp=temp->next;
    }
    temp->next=node;
    return 1;
}
struct list_element* remove_node(uint32_t node_num, struct list_element* head){
    if(head==0||head->next==0)
    return head;
    struct list_element* temp=head;
    if(node_num==0){
        head=head->next;
        free_mem((uint32_t)temp);
        return head;
    }
    for(uint32_t i=1;i<node_num;i++){
        temp=temp->next;
        if(temp==0)
        return head;
    }
    struct list_element* temp1 = temp->next;
    temp->next = temp->next->next;
    free_mem((uint32_t)temp1);
    return head;
}