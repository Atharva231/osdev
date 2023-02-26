void kmain();
void start(){
    kmain();
}
#include "./mgmt/memory.c"
#include "./drivers/screen.c"
#include "./drivers/timer.c"
#include "./drivers/keyboard.c"
#include "./drivers/disk.c"
#include "./mgmt/virtual_memory_mgmt.c"
#include "./mgmt/filesystem.c"
#include "./mgmt/program_loader.c"
#include "./mgmt/system_call.c"
#include "./drivers/interrupts.c"
#include <stdint.h>
#include <stdbool.h>
#define SYSCALL_BUFFER_ADDR 0xFFFE0

extern void jump(int addr);
extern void halt();
void kmain(){
    idt_init();
    kb_init();
    set_print_status(true);
    //set_pit_count(1000);
    //timer_init();
    heap_init(4096);
    vmm_init();
    uint8_t string[]="Atharva ";
    uint8_t delim[]=",";
    clear_screen();
    print_text(string);
    uint8_t str[512];
    for(uint16_t i=0;i<512;i++)
	str[i]=0;
    struct file_list_element* file=(struct file_list_element*)str;
    struct dir_list_element* dir;
/*    file->delim=' ';
    refresh_name(file->file_name, false);
    file->file_name[0]='a';
    file->file_name[1]='t';
    file->file_name[2]='h';
    file->file_name[3]='a';
    file->file_name[4]='r';
    file->file_name[5]='v';
    file->file_name[6]='a';
    file=(struct file_list_element*)&str[sizeof(struct file_list_element)];
    file->delim=',';
    refresh_name(file->file_name, false);
    file->file_name[0]='k';
    file->file_name[1]='i';
    file->file_name[2]='t';
    file->file_name[3]='t';
    file->file_name[4]='u';
    dir=(struct dir_list_element*)&str[sizeof(struct file_list_element)*2];
    dir->delim='/';
    refresh_name(dir->dir_name, true);
    dir->dir_name[0]='d';
    dir->dir_name[1]='i';
    dir->dir_name[2]='r';
    dir->dir_name[3]='1';
    file=(struct file_list_element*)&str[96];
    file->delim='|';
    refresh_name(file->file_name, false);
    file->file_name[0]='s';
    file->file_name[1]='u';
    file->file_name[2]='b';
    file->file_name[3]='f';
    file->file_name[4]='i';
    file->file_name[5]='l';
    file->file_name[6]='e';
    str[128]='!';*/
/*    str[0]='!';
    write_sectors(0x1F,0x1,(uint16_t*)str);
    filesystem_init(0x1F, 0x2B);*/
    clear_screen();
    print_text(string);
    /*uint8_t temp_name[19];
    refresh_name(temp_name,false);
    temp_name[0]='a';
    temp_name[1]='t';
    temp_name[2]='h';
    temp_name[3]='a';
    temp_name[4]='~';
    temp_name[5]='~';
    temp_name[6]='~';
    create_file(temp_name);
    list_files();
    temp_name[0]='d';
    temp_name[1]='i';
    temp_name[2]='r';
    temp_name[3]='2';
    temp_name[4]='~';
    temp_name[5]='~';
    temp_name[6]='~';
    create_dir(temp_name);
    list_dirs();
    update_fat(dir_temp);*/
    read_sectors((uint16_t*)0x800000, 0x2D, 0xF);
    uint32_t* k=get_syscall_buff();
    k[0]=21;
    k[1]=0x8000F4;
    k[2]=0x810000;
    asm("int $0x80");
    k[0]=19;
    k[1]=0x8000F4;
    asm("int $0x80");
    print_num(k[0]);
    print_text(delim);
    jump(k[0]);
    halt();
}