; master boot record file
; File used to define the master boot record (512 byte boot sector)

[bits 16]
[org 0x7c00]

; where to load the kernel to
KERNEL_OFFSET equ 0x1000
KERNEL_SECTORS equ 16

; Make sure all segment registers are sane (BIOS values vary)
cli
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x9000
sti

; BIOS sets boot drive in 'dl' store for later use
mov [BOOT_DRIVE], dl

call load_kernel
call switch_to_32bit

jmp $

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET   ; bx -> destination
    mov dh, KERNEL_SECTORS  ; dh -> num sectors
    mov dl, [BOOT_DRIVE]    ; dl -> disk
    call disk_load
    ret

[bits 32]
BEGIN_32BIT:
    ; Jump to the loaded kernel at physical/linear 0x1000
    mov eax, KERNEL_OFFSET
    jmp eax
    jmp $               ; loop just in case the kernel returns

; boot drive variable
BOOT_DRIVE db 0

%include "disk.asm"
%include "gdt.asm"
%include "switch-to-32bit.asm"

; padding
times 510 - ($-$$) db 0

; magic number
dw 0xaa55