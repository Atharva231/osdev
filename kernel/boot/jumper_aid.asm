global start
global port_byte_in
global port_byte_out
global port_word_in
global port_word_out
global jump
global print_char 
global halt
port_byte_in:
	mov edx, [esp + 4]
    xor eax, eax
	in al, dx	
	ret

port_byte_out:
	mov   edx, [esp + 4]    
	mov   al, [esp + 4 + 4]  
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

jump:
    jmp [esp + 4]

halt:
    jmp $
print_char:
    mov   ebx, [esp + 4]
	mov   al, [esp + 4 + 4]
    mov [ebx], al
    ret