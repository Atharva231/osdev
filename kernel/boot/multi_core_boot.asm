[org 0x7C00]
mov [BOOT_DRIVE], dl
mov bp, 0xA000
mov sp, bp

;Clear screen
mov ax, 03h
int 10h

mov ebx, AP_CODE ;destination memory address
mov dl, [BOOT_DRIVE]
mov dh, 0x01   ;number of sectors to read
mov cl , 0x02 ; Start reading after the boot sector
call disk_load

mov ebx, KERNEL_OFFSET
mov dl, [BOOT_DRIVE]
mov dh, 0x03
mov cl , 0x03
call disk_load
call switch_to_pm
jmp $

%include "diskread.asm"
%include "print_string.asm"
%include "switch_to_pm.asm"
%include "gdt.asm"

[bits 32]
BEGIN_PM:
  mov ebp, 0x9EFFF
  mov esp, ebp
  mov [0xb8000], byte 'B'
  mov [0xb8002], byte 'S'
  mov [0xb8004], byte 'P'

  call enable_lapic
  ;INIT
  call lapic_send_init
  mov cx, WAIT_10_ms
  call us_wait
  ;SIPI
  call lapic_send_sipi
  mov cx, WAIT_200_us
  call us_wait
  ;SIPI
  call lapic_send_sipi
    
  mov ecx, 0x1B
  rdmsr
  shr eax, 8
  and eax, 0x01
  cmp eax, 0x01
  jne AP_CODE
  jmp KERNEL_OFFSET

enable_lapic:
  mov ecx, IA32_APIC_BASE_MSR
  rdmsr
  or ah, 0x09        ;bit11: APIC GLOBAL Enable/Disable
  wrmsr
  ;Mask off lower 12 bits to get the APIC base address
  and ah, 0xF0
  mov DWORD [APIC_BASE], eax
  ;Newer processors enables the APIC through the Spurious Interrupt Vector register
  mov [APIC_REG_SIV], BYTE 0xFF
  ret

lapic_send_sipi:
  mov eax, DWORD [APIC_BASE]
  ;Destination field is set to 0 has we will use a shorthand
  xor ebx, ebx
  mov DWORD [APIC_REG_ICR_HIGH], ebx
  mov ebx, 0xC4608
  mov DWORD [APIC_REG_ICR_LOW], ebx  ;Writing the low DWORD sent the IPI
  ret

 lapic_send_init:
  mov eax, DWORD [APIC_BASE]
  ;Destination field is set to 0 has we will use a shorthand
  xor ebx, ebx
  mov DWORD [APIC_REG_ICR_HIGH], ebx
  mov ebx, 0xC4500
  mov DWORD [APIC_REG_ICR_LOW], ebx  ;Writing the low DWORD sent the IPI
  ret

;CX = Wait (in ms) Max 65536 us (=0 on input)
us_wait:
    mov dx, 80h               ;POST Diagnose port, 1us per IO
    xor si, si
    rep outsb
    ret
IA32_APIC_BASE_MSR  EQU 1bh
APIC_REG_SIV        EQU 0xFEE000F0
APIC_REG_ICR_LOW    EQU 0xFEE00300
APIC_REG_ICR_HIGH   EQU 0xFEE00310
APIC_REG_ID         EQU 0xFEE00020
KERNEL_OFFSET       equ 0x9000
AP_CODE             equ 0x8000
WAIT_10_ms          equ 10000d
WAIT_200_us         equ 200d
APIC_BASE           dd  0xFEE00000
BOOT_DRIVE          db 0
times 510-($-$$) db 0
dw 0xAA55
