GLOBAL rtc_readValue

section .text

rtc_readValue:
	mov al, dil
	out 70h, al
	in al, 71h
	ret