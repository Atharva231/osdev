#include <stdint.h>
#include <stdbool.h>
extern uint32_t resume_prg();
uint32_t id_count=0, prg_addr, prg_id;
struct Process_Control_Block* get_pcb_head();
struct Process_Control_Block{
	uint32_t pid;
	uint32_t pstat;
	uint8_t apic_id;
	uint32_t text_start;
	uint32_t text_size;
	uint32_t bss_start;
	uint32_t stack_start;
	uint32_t esp;
	uint32_t pflags;
	uint32_t entry_addr;
	struct Process_Control_Block* next;
};
struct Process_Control_Block* get_pcb(struct Process_Control_Block* pcb_head, uint32_t id){
	struct Process_Control_Block* temp=pcb_head;
	for(;temp!=0;temp=temp->next){
		if(temp->pid==id){
			return temp;
		}
	}
	return (struct Process_Control_Block*)1;
}
struct Process_Control_Block* get_live_pcb(struct Process_Control_Block* pcb_head){
	struct Process_Control_Block* temp=pcb_head;
	for(;temp!=0;temp=temp->next){
		if(temp->pstat==2 && temp->apic_id==get_apic_id()){
			return temp;
		}
	}
	return 0;
}
uint32_t test_save_state(uint32_t int_id, uint32_t addr, uint32_t flag, uint32_t esp){
	struct Process_Control_Block* pcb=get_live_pcb(get_pcb_head());
	if(pcb!=0){
		pcb->esp=esp;
		pcb->entry_addr=addr;
		pcb->pflags=flag;
		/*print_num_hex(pcb->pid);
		print_text(",");
		print_num_hex(addr);
		print_text(",");
		print_num_hex(flag);
		print_text(",");
		print_num_hex(esp);
		print_text(",");
		print_num_hex(pcb->stack_start);
		print_text(",");*/
	}
	if(int_id==2){
		pcb->pstat=1;
		return prg_addr;
	}
}

uint32_t resume_prg(){
	struct Process_Control_Block* pcb=get_pcb(get_pcb_head(), prg_id);
	if(pcb==0){
		return 0;
	}
	if(pcb->pstat!=1){
		return 0;
	}
	pcb->pstat=2;
	pcb->apic_id=get_apic_id();
	asm("mov %0, %%esp"::"r"(pcb->esp));
	asm("popal");
	asm("iretl");
	return 1;
}

uint32_t save_state(uint32_t addr, uint32_t flag, uint32_t esp){
	struct Process_Control_Block* pcb=get_live_pcb(get_pcb_head());
	if(pcb!=0){
		pcb->esp=esp;
		pcb->entry_addr=addr;
		pcb->pflags=flag;
		/*print_num_hex(pcb->pid);
		print_text(",");
		print_num_hex(addr);
		print_text(",");
		print_num_hex(flag);
		print_text(",");
		print_num_hex(esp);
		print_text(",");
		print_num_hex(pcb->stack_start);
		print_text(",");*/
	}
	return prg_addr;
}

struct Process_Control_Block* create_pcb_list(){
	struct Process_Control_Block* pcb_head=(struct Process_Control_Block*)mem_alloc(sizeof(struct Process_Control_Block));
	id_count+=1;
	pcb_head->pid=id_count;
	pcb_head->next=0;
	return pcb_head;
}
struct Process_Control_Block* add_pcb_node(struct Process_Control_Block* pcb_head){
	if(pcb_head==0)
		return 0;
	uint32_t t=mem_alloc(sizeof(struct Process_Control_Block));
	if(t==0)
		return 0;
	struct Process_Control_Block* temp=pcb_head;
	while(temp->next!=0){
		temp=temp->next;
	}
	temp->next=(struct Process_Control_Block*)t;
	id_count+=1;
	temp->next->pid=id_count;
	return temp;
}
uint8_t remove_pcb_node(struct Process_Control_Block* pcb_head, struct Process_Control_Block* pcb_node){
	if(pcb_head==0 || pcb_node==0)
		return 0;
	uint32_t t=(uint32_t)pcb_node;
	if(pcb_node==pcb_head){
		pcb_head=pcb_node;
		free_mem(t, sizeof(struct Process_Control_Block));
		return 1;
	}
	struct Process_Control_Block* temp=pcb_head;
	while(temp->next!=pcb_node){
		temp=temp->next;
	}
	if(pcb_node->next==0){
		temp->next=0;
	}
	else if(pcb_node->next!=0){
		temp->next=pcb_node->next;
	}
	free_mem(t, sizeof(struct Process_Control_Block));
	id_count-=1;
	return 1;
}
void set_prg_addr(uint32_t addr){
	prg_addr=addr;
}
