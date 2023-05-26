[org 0x7C00]
mov [BOOT_DRIVE], dl
mov bp, 0x9000
mov sp, bp

call switch_to_pm
jmp $

%include "switch_to_pm.asm"
%include "gdt.asm"

[bits 32]
BEGIN_PM:
    mov ebp, 0x8FFFF
    mov esp, ebp
    mov [0xb8000],byte "A"
    mov [0xb8002],byte "P"
    jmp $

BOOT_DRIVE db 0
times 510-($-$$) db 0
dw 0xAA55
