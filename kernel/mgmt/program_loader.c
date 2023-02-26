#include <stdint.h>
#include <stdbool.h>
#include <elf.h>

Elf32_Ehdr* elf_hdr;
Elf32_Phdr* prg_hdr;
Elf32_Shdr* sec_entry;
Elf32_Shdr* text_sec;
Elf32_Shdr* data_sec;
Elf32_Shdr* reloc_sec;
Elf32_Shdr* symtab_sec;
Elf32_Shdr* bss_sec;
Elf32_Shdr* strtab_sec;
Elf32_Sym* sym_entry;
Elf32_Rel* rel_entry;
uint32_t perm_parser(uint32_t flag){
    switch(flag){
	case 7:
	case 6:
	case 3:
	case 2:
	return 3;

	case 5:
	case 4:
	case 1:
	return 1;
    }
}
uint32_t load_elf(uint32_t addr){
    elf_hdr=(Elf32_Ehdr*)addr;
    if(elf_hdr->e_ident[1]!='E' || elf_hdr->e_ident[2]!='L' || elf_hdr->e_ident[3]!='F')
	   return 0;
    else if(elf_hdr->e_type!=2){
	   return 1;
    }
    uint16_t flag=0;
    uint8_t* src;
    uint8_t* dst;
    uint8_t delim[]=",";
    uint32_t memsz,c=0;
    uint8_t* sec_ptr1;
    sec_entry=(Elf32_Shdr*)(addr+elf_hdr->e_shoff+(elf_hdr->e_shstrndx*elf_hdr->e_shentsize));
    uint8_t* shstrtab=(uint8_t*)sec_entry->sh_offset+addr;
    for(uint8_t i=0;i<elf_hdr->e_shnum;i++){
        sec_entry=(Elf32_Shdr*)(addr+elf_hdr->e_shoff+(i*elf_hdr->e_shentsize));
        switch(shstrtab[(uint32_t)sec_entry->sh_name+1]){
        case 't':
            text_sec=sec_entry;
            ++c;
            break;
        case 'd':
            data_sec=sec_entry;
            ++c;
            break;
        case 'r':
            if(shstrtab[(uint32_t)sec_entry->sh_name+5]=='t'){
                reloc_sec=sec_entry;
                ++c;
            }
            break;
        case 's':
            if(shstrtab[(uint32_t)sec_entry->sh_name+2]=='y'){
                symtab_sec=sec_entry;
                ++c;
            }
            else if(shstrtab[(uint32_t)sec_entry->sh_name+2]=='t'){
                strtab_sec=sec_entry;
            }
            break;
        case 'b':
            bss_sec=sec_entry;
            ++c;
            break;
        }
    }
    if(c==0)
       return 2;
    uint32_t prg_entry_addr=0;
    bool f=true;
    for(uint8_t i=0;i<symtab_sec->sh_size/sizeof(Elf32_Sym);i++){
        sym_entry=(Elf32_Sym*)(addr+symtab_sec->sh_offset+i*sizeof(Elf32_Sym));
        switch(sym_entry->st_info&0xF){
        case 0x02:
            sec_ptr1=addr+strtab_sec->sh_offset+(uint8_t*)sym_entry->st_name;
            print_text(sec_ptr1);
            print_text(delim);
            c=0;
            for(uint8_t i=0;i<sec_ptr1[i]!=0;i++){
                c+=1;
            }
            if(c==4){
                if(sec_ptr1[0]=='m'){
                    if(sec_ptr1[1]=='a'){
                        if(sec_ptr1[2]=='i'){
                            if(sec_ptr1[3]=='n'){
                                f=false;
                                prg_entry_addr=sym_entry->st_value;
                            }
                        }
                    }
                }
            }
        }
    }
    if(f){
        return 4;
    }
    for(uint8_t i=0;i<elf_hdr->e_shnum;i++){
        sec_entry=(Elf32_Shdr*)(addr+elf_hdr->e_shoff+(i*elf_hdr->e_shentsize));
        if(sec_entry->sh_addr==0)
            continue;
        dst=(uint8_t*)sec_entry->sh_addr;
        src=(uint8_t*)(addr+sec_entry->sh_offset);
        for(uint32_t j=0;j<sec_entry->sh_size;j++){
            dst[j]=src[j];
        }
    }
    return prg_entry_addr;
}

uint32_t link_load_elf(uint32_t addr, uint32_t reloc_addr){
    elf_hdr=(Elf32_Ehdr*)addr;
    if(elf_hdr->e_ident[1]!='E' || elf_hdr->e_ident[2]!='L' || elf_hdr->e_ident[3]!='F')
        return 0;
    else if(elf_hdr->e_type!=1)
        return 1;
    uint32_t c=0,sym_type,temp;
    uint32_t sym_addr;
    uint32_t* sec_ptr;
    uint8_t* sec_ptr1;
    uint8_t* sec_ptr2;
    uint8_t delim[]=",";
    sec_entry=(Elf32_Shdr*)(addr+elf_hdr->e_shoff+(elf_hdr->e_shstrndx*elf_hdr->e_shentsize));
    uint8_t* shstrtab=(uint8_t*)sec_entry->sh_offset+addr;
    for(uint8_t i=0;i<elf_hdr->e_shnum;i++){
    	sec_entry=(Elf32_Shdr*)(addr+elf_hdr->e_shoff+(i*elf_hdr->e_shentsize));
        switch(shstrtab[(uint32_t)sec_entry->sh_name+1]){
        case 't':
            text_sec=sec_entry;
            ++c;
            break;
        case 'd':
            data_sec=sec_entry;
            ++c;
            break;
        case 'r':
            if(shstrtab[(uint32_t)sec_entry->sh_name+5]=='t'){
                reloc_sec=sec_entry;
                ++c;
            }
            break;
        case 's':
            if(shstrtab[(uint32_t)sec_entry->sh_name+2]=='y'){
                symtab_sec=sec_entry;
                ++c;
            }
            else if(shstrtab[(uint32_t)sec_entry->sh_name+2]=='t'){
                strtab_sec=sec_entry;
            }
            break;
        case 'b':
            bss_sec=sec_entry;
            ++c;
            break;
        }
    }
    if(c==0)
	   return 2;
    uint32_t prg_entry_addr=0;
    bool f=true;
    for(uint8_t i=0;i<symtab_sec->sh_size/sizeof(Elf32_Sym);i++){
        sym_entry=(Elf32_Sym*)(addr+symtab_sec->sh_offset+i*sizeof(Elf32_Sym));
        switch(sym_entry->st_info&0xF){
        case 0x02:
            sec_ptr1=addr+strtab_sec->sh_offset+(uint8_t*)sym_entry->st_name;
            print_text(sec_ptr1);
            print_text(delim);
            c=0;
            for(uint8_t i=0;i<sec_ptr1[i]!=0;i++){
                c+=1;
            }
            if(c==4){
                if(sec_ptr1[0]=='m'){
                    if(sec_ptr1[1]=='a'){
                        if(sec_ptr1[2]=='i'){
                            if(sec_ptr1[3]=='n'){
                                f=false;
                                prg_entry_addr=reloc_addr+sym_entry->st_value;
                            }
                        }
                    }
                }
            }
        }
    }
    if(f){
        return 3;
    }
    sec_ptr2=(uint8_t*)reloc_addr;
    sec_ptr1=(uint8_t*)(addr+text_sec->sh_offset);
    for(uint32_t i=0;i<text_sec->sh_size;i++){
        sec_ptr2[i]=sec_ptr1[i];
    }
    sec_ptr2=(uint8_t*)(reloc_addr+text_sec->sh_size);
    sec_ptr1=(uint8_t*)(addr+data_sec->sh_offset);
    for(uint32_t i=0;i<data_sec->sh_size;i++){
        sec_ptr2[i]=sec_ptr1[i];
    }
    c=0;
    for(uint8_t i=0;i<(reloc_sec->sh_size)/(reloc_sec->sh_entsize);i++){
       rel_entry=(Elf32_Rel*)(addr+reloc_sec->sh_offset+(i*8));
       sym_type=rel_entry->r_info&0xFF;
       sym_addr=(rel_entry->r_info>>8);
       sym_entry=(Elf32_Sym*)(addr+symtab_sec->sh_offset+sym_addr*sizeof(Elf32_Sym));
       if(sym_type==0x01){
        if(sym_entry->st_shndx==0xfff2){
            sec_ptr1=(uint8_t*)(reloc_addr+text_sec->sh_size+data_sec->sh_size+c);
            for(uint32_t i=0;i<sym_entry->st_size;i++){
                sec_ptr1[i]=0;
            }
            sym_entry->st_value=c;
            c+=sym_entry->st_size;
        }
       }
    }
    for(uint8_t i=0;i<(reloc_sec->sh_size)/(reloc_sec->sh_entsize);i++){
	   rel_entry=(Elf32_Rel*)(addr+reloc_sec->sh_offset+(i*8));
       sym_type=rel_entry->r_info&0xFF;
       sym_addr=(rel_entry->r_info>>8);
       sym_entry=(Elf32_Sym*)(addr+symtab_sec->sh_offset+sym_addr*sizeof(Elf32_Sym));
       sec_ptr=(uint32_t*)(reloc_addr+rel_entry->r_offset);
       if(sym_type==0x01){
        if(sym_entry->st_shndx==0xfff2){
            temp=*sec_ptr;
            *sec_ptr=(reloc_addr+text_sec->sh_size+data_sec->sh_size+sym_entry->st_value)+temp;
        }
        else{
            temp=*sec_ptr;
            *sec_ptr=(reloc_addr+text_sec->sh_size+sym_entry->st_value)+temp;
        }
       }
       else if(sym_type==0x02){
        *sec_ptr=(sym_entry->st_value-(rel_entry->r_offset+4));
       }
    }
    return prg_entry_addr;
}

uint32_t link_elf(uint32_t addr, uint32_t reloc_addr){
    elf_hdr=(Elf32_Ehdr*)addr;
    if(elf_hdr->e_ident[1]!='E' || elf_hdr->e_ident[2]!='L' || elf_hdr->e_ident[3]!='F')
        return 0;
    else if(elf_hdr->e_type!=1)
        return 1;
    uint32_t c=0,sym_type,temp;
    uint32_t sym_addr;
    uint32_t* sec_ptr;
    uint8_t* sec_ptr1;
    uint8_t* sec_ptr2;
    uint8_t delim[]=",";
    sec_entry=(Elf32_Shdr*)(addr+elf_hdr->e_shoff+(elf_hdr->e_shstrndx*elf_hdr->e_shentsize));
    uint8_t* shstrtab=(uint8_t*)sec_entry->sh_offset+addr;
    for(uint8_t i=0;i<elf_hdr->e_shnum;i++){
        sec_entry=(Elf32_Shdr*)(addr+elf_hdr->e_shoff+(i*elf_hdr->e_shentsize));
        switch(shstrtab[(uint32_t)sec_entry->sh_name+1]){
        case 't':
            text_sec=sec_entry;
            ++c;
            break;
        case 'd':
            data_sec=sec_entry;
            ++c;
            break;
        case 'r':
            if(shstrtab[(uint32_t)sec_entry->sh_name+5]=='t'){
                reloc_sec=sec_entry;
                ++c;
            }
            break;
        case 's':
            if(shstrtab[(uint32_t)sec_entry->sh_name+2]=='y'){
                symtab_sec=sec_entry;
                ++c;
            }
            else if(shstrtab[(uint32_t)sec_entry->sh_name+2]=='t'){
                strtab_sec=sec_entry;
            }
            break;
        case 'b':
            bss_sec=sec_entry;
            ++c;
            break;
        }
    }
    if(c==0)
       return 2;
    uint32_t prg_entry_addr=0;
    bool f=true;
    for(uint8_t i=0;i<symtab_sec->sh_size/sizeof(Elf32_Sym);i++){
        sym_entry=(Elf32_Sym*)(addr+symtab_sec->sh_offset+i*sizeof(Elf32_Sym));
        switch(sym_entry->st_info&0xF){
        case 0x02:
            sec_ptr1=addr+strtab_sec->sh_offset+(uint8_t*)sym_entry->st_name;
            print_text(sec_ptr1);
            print_text(delim);
            c=0;
            for(uint8_t i=0;i<sec_ptr1[i]!=0;i++){
                c+=1;
            }
            if(c==4){
                if(sec_ptr1[0]=='m'){
                    if(sec_ptr1[1]=='a'){
                        if(sec_ptr1[2]=='i'){
                            if(sec_ptr1[3]=='n'){
                                f=false;
                                prg_entry_addr=addr+text_sec->sh_offset+sym_entry->st_value;
                            }
                        }
                    }
                }
            }
        }
    }
    if(f){
        return 3;
    }
    c=0;
    for(uint8_t i=0;i<(reloc_sec->sh_size)/(reloc_sec->sh_entsize);i++){
       rel_entry=(Elf32_Rel*)(addr+reloc_sec->sh_offset+(i*8));
       sym_type=rel_entry->r_info&0xFF;
       sym_addr=(rel_entry->r_info>>8);
       sym_entry=(Elf32_Sym*)(addr+symtab_sec->sh_offset+sym_addr*sizeof(Elf32_Sym));
       if(sym_type==0x01){
        if(sym_entry->st_shndx==0xfff2){
            sec_ptr1=(uint8_t*)(reloc_addr+text_sec->sh_size+data_sec->sh_size+c);
            sym_entry->st_value=c;
            c+=sym_entry->st_size;
        }
       }
    }
    text_sec->sh_addr=reloc_addr;
    data_sec->sh_addr=reloc_addr+text_sec->sh_size;
    bss_sec->sh_addr=data_sec->sh_addr+data_sec->sh_size;
    for(uint8_t i=0;i<(symtab_sec->sh_size/symtab_sec->sh_entsize);i++){
        sym_entry=(Elf32_Sym*)(addr+symtab_sec->sh_offset+i*sizeof(Elf32_Sym));
        sym_type=sym_entry->st_info&0xF;
        if(sym_type==0x1){
            if(sym_entry->st_shndx==0xfff2){
                sym_entry->st_value+=bss_sec->sh_addr;
            }
            else{
                sym_entry->st_value+=data_sec->sh_addr;
            }
        }
        else if(sym_type==0x2){
            sym_entry->st_value+=text_sec->sh_addr;
        }
    }
    for(uint8_t i=0;i<(reloc_sec->sh_size)/(reloc_sec->sh_entsize);i++){
       rel_entry=(Elf32_Rel*)(addr+reloc_sec->sh_offset+(i*8));
       sym_type=rel_entry->r_info&0xFF;
       sym_addr=(rel_entry->r_info>>8);
       sym_entry=(Elf32_Sym*)(addr+symtab_sec->sh_offset+sym_addr*sizeof(Elf32_Sym));
       sec_ptr=(uint32_t*)(addr+text_sec->sh_offset+rel_entry->r_offset);
       if(sym_type==0x01){
        if(sym_entry->st_shndx==0xfff2){
            temp=*sec_ptr;
            *sec_ptr=sym_entry->st_value+temp;
        }
        else{
            temp=*sec_ptr;
            *sec_ptr=sym_entry->st_value+temp;
        }
       }
       else if(sym_type==0x02){
        *sec_ptr=((sym_entry->st_value-text_sec->sh_addr)-(rel_entry->r_offset+4));
       }
    }
    elf_hdr->e_type=2;
    return prg_entry_addr;
}
