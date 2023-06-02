[org 0x8000]

[bits 16]
switch_to_pm:
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp codeseg:init_pm

[bits 32]
init_pm:
    mov ax, dataseg
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    jmp set_ap

gdt_nulldesc:
    dd 0
    dd 0
gdt_codedesc:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00
gdt_datadesc:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    gdt_size:
        dw gdt_end - gdt_nulldesc-1
        dd gdt_nulldesc

codeseg equ gdt_codedesc - gdt_nulldesc
dataseg equ gdt_datadesc - gdt_nulldesc

[bits 32]
set_ap:

    mov eax, 0x0
    mov ax, WORD [counter]
    .try:
        mov bx, ax
        inc bx
        lock cmpxchg WORD [counter], bx
    jnz .try
    mov cx, ax            ;Save this unique id

    ;set_stack  
    mov ebx, 0x10000
    mul ebx
    mov ebx, 0x90000
    sub ebx, eax
    mov ebp, ebx
    mov esp, ebp

    ;print 'AP-'
    mov eax, ecx
    mov ebx, 160d
    mul ebx
    mov edx, 0xb8000
    add eax, edx
    mov edi, eax
    mov [edi], byte 'A'
    mov [edi + 2], byte 'P'
    mov [edi + 4], byte '-'
    mov eax, 0x06
    add edi, eax

    ;get initial apic id
    push ecx
    mov eax, 0x01
    cpuid
    mov eax, ebx
    shr eax, 24

    ;enter apic in apic id list
    mov ebx, apic_id_list
    pop ecx
    push ecx
    push eax
    mov eax, 0x04
    mul ecx
    add ebx, eax
    pop eax
    lock add [ebx], eax

    ;print apic id
    call print_num

    jmp 0x9000

print_num:
    mov edx, 0
    mov ebx, 10d
    div ebx
    mov ecx, [digits + edx]
    mov [edi], cl
    inc edi
    inc edi
    mov ebx, 0
    cmp eax, ebx
    jnz print_num
    ret

counter             dw 1
digits              db "0123456789abcdef"
apic_id_list        dd 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0