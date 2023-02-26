print_string:
    mov ah, 0x0e
    .loop:
        mov al, [bx]
        cmp al, 0
        je .return
        int 0x10
        inc bx
        jmp .loop
    .return ret