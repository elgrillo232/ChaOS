
[bits 32]

section .text

global _start
global ___main
extern _main

_start:
	call _main

.hang:
	cli
	hlt
	jmp .hang

; MinGW GCC may emit a call/reference to __main even for freestanding builds.
; In COFF symbol form that becomes ___main. Provide a no-op stub.
___main:
	ret
