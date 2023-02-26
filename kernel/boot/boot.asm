[org 0x7C00]
KERNEL_OFFSET equ 0x7E00
mov [BOOT_DRIVE], dl
mov bp, 0x9000
mov sp, bp

mov ebx, KERNEL_OFFSET
mov dl, [BOOT_DRIVE]
mov dh, 0x3
call disk_load
call switch_to_pm
jmp $

%include "diskread.asm"
%include "print_string.asm"
%include "switch_to_pm.asm"
%include "gdt.asm"

[bits 32]
BEGIN_PM:
    mov [0xb8000],byte "h"
    mov [0xb8002],byte "i"
    jmp KERNEL_OFFSET
    jmp $
MSG_LOAD_KERNEL:
    db " Loading kernel into memory. " , 0

BOOT_DRIVE db 0
times 510-($-$$) db 0
dw 0xAA55
