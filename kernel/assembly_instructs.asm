[bits 32]
global read_port
global write_port
global print_char
global exec_prg
global resume_prg
global halt
global load_idt
global load_gdt
global timer_handler
global system_call_handler
global enable_interrupts
global disable_interrupts
global keyboard_handler
global port_word_in
global port_word_out
global port_dword_in
global port_dword_out
global page_fault_handler
global enablePaging
global loadPageDirectory
global enable_PSE
global read_cr2
[extern keyboard_get_print]
[extern save_state]
[extern timer_task]
[extern system_call_task]
[extern page_fault_task]
[extern print_num]
read_port:
	mov edx, [esp + 4]
    xor eax, eax
	in al, dx	
	ret
write_port:
	mov   edx, [esp + 4]    
	mov   al, [esp + 8]  
	out   dx, al  
	ret
port_word_in:
    mov edx, [esp + 4]
    in ax, dx
    ret

port_word_out:
    mov edx, [esp + 4]
    mov ax, [esp + 8]
    out dx, ax
    ret

port_dword_in:
    mov edx, [esp + 4]
    in eax, dx
    ret

port_dword_out:
    mov edx, [esp + 4]
    mov eax, [esp + 8]
    out dx, eax
    ret

print_char:
    mov   ebx, [esp + 4]
	mov   al, [esp + 8]
    mov [ebx], al
    ret

exec_prg:
    pushad
    mov eax, [esp + 36]
    mov ebp, [esp + 40]
    mov ebx, esp
    mov esp, ebp
    push ebx
    call eax
    pop esp
    popad
    ret

resume_prg:
    mov eax, [esp + 4]
    mov esp, eax
    popfd
    popad
    ret

halt:
    jmp $

load_idt:
  	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret

load_gdt:
    cli
    mov edx, [esp + 4]
    lgdt [edx]
    ret

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

keyboard_handler:                 
	pushad
    cld
    call keyboard_get_print
    mov ebx, 1
    cmp eax, ebx
    jne chgPrg
	popad
    iretd
    chgPrg:
        push esp
        push dword[esp + 40]
        push dword[esp + 40]
        call save_state
        pop ecx
        pop ecx
        pop ecx
        push dword[esp + 40]
        push cs
        push eax
        iretd

system_call_handler:
    pushad
    cld
    call system_call_task
    popad
    iretd
    
timer_handler:
    pushad
    cld
    call timer_task
    popad
    iretd

page_fault_handler:
    pushad
    cld
    call page_fault_task
    popad
    add esp, 4
    iretd

loadPageDirectory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

enablePaging:
    mov eax, 0x80000001
    mov cr0, eax
    ret

enable_PSE:
    mov eax, 0x00000010
    mov cr4, eax
    ret

read_cr2:
    mov eax, cr2
    ret

