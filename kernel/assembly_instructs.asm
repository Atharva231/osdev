global port_byte_in
global port_byte_out
global print_char
global exec_prg
global halt
global load_idt
global load_gdt
global timer_handler
global system_call_handler
global enable_interrupts
global disable_interrupts
global keyboard_handler
global cont_switch
global page_fault_handler
global pause_handler
global ap_task_handler
global get_idt
global port_word_in
global port_word_out
global port_dword_in
global port_dword_out
global enablePaging
global loadPageDirectory
global enable_PSE
global read_cr2
global get_msr
global set_msr
[extern keyboard_get_print]
[extern test_save_state]
[extern save_state]
[extern timer_task]
[extern system_call_task]
[extern page_fault_task]
[extern pause]
[extern ap_task]
[extern print_num]
[extern print_num_hex]
[extern send_EOI]
port_byte_in:
	mov edx, [esp + 4]
    xor eax, eax
	in al, dx	
	ret
port_byte_out:
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
    mov ecx, [ebp]  ;checking for page fault
    mov ebx, esp
    mov esp, ebp
    push ebx
    call eax
    pop esp
    popad
    ret

halt:
    hlt
    jmp halt

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
    call send_EOI
	popad
    iretd
    chgPrg:
        push esp
        push dword[esp + 44]
        push dword[esp + 40]
        push eax
        call test_save_state
        pop ebx
        pop ecx
        pop ecx
        pop ecx
        push dword[esp + 40]
        push cs
        push eax
        call send_EOI
        iretd
        
cont_switch:
    pushad
    push esp
    push dword [esp + 44]
    push dword [esp + 40]
    call save_state
    pop ecx
    pop ecx
    pop ecx
    push dword[esp + 40]
    push cs
    push eax
    call send_EOI
    iretd

system_call_handler:
    pushad
    cld
    call system_call_task
    call send_EOI
    popad
    iretd
    
timer_handler:
    pushad
    cld
    call timer_task
    call send_EOI
    popad
    iretd

page_fault_handler:
    pushad
    cld
    call page_fault_task
    call send_EOI
    popad
    add esp, 4
    iretd

pause_handler:
    pushad
    cld
    call pause
    call send_EOI
    popad
    iretd

ap_task_handler:
    pushad
    cld
    mov dword [esp + 32], ap_task
    call send_EOI
    popad
    iretd

get_idt:
    sidt [esp - 8]
    mov eax, dword [esp - 8]
    mov ecx, dword [esp - 4]
    shr eax, 16
    shl ecx, 16
    or eax, ecx
    ret

loadPageDirectory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

enablePaging:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret

enable_PSE:
    mov eax, 0x00000010
    mov cr4, eax
    ret

read_cr2:
    mov eax, cr2
    ret

get_msr:
    mov ecx, [esp + 4]
    mov ebx, [esp + 8]
    rdmsr
    mov [ebx], eax
    mov [ebx + 4], edx
    ret

set_msr:
    mov ecx, [esp + 4]
    mov ebx, [esp + 8]
    mov eax, [ebx]
    mov edx, [ebx + 4]
    wrmsr
    ret

