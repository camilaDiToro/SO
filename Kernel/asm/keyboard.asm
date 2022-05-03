GLOBAL kbd_readKey

section .text

kbd_readKey:
	xor rax, rax
.loop:
	in al, 0x64      ; Read 8042 status register. Can be read at any time.
	and al, 0x01     ; Output register 60h should only be read IIF Bit 0 of status port is set to 1.
	cmp al, 0
	je .loop
	in al, 0x60
	
	ret