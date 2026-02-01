[bits 16]

disk_load:
    pusha
    push dx

    ; Reset disk system (AH=0) before reading
    xor ax, ax
    int 0x13

    mov ah, 0x02    ; read mode
    mov al, dh      ; read dh number of sectors
    mov cl, 0x02    ; starts from sector 2
                    ; (as sector 1 is our boot sector)
    mov ch, 0x00    ; cylinder 0
    mov dh, 0x00    ; head 0

    ; dl = driver number is set as input to disk_load
    ; es:bx = buffer pointer is set as input too

    int 0x13        ; BIOS interrupt
    jc disk_error   ; check carry bit for error

    pop dx          ; gets origional number of sectors to read back
    cmp al, dh      ; BIOS sets 'al' to the num of sectors actually read
                    ; compare it to 'dh' and error out if ther are !=
    jne sectors_error
    popa
    ret

disk_error:
    jmp disk_loop   ; if error jump to disk_loop

sectors_error:
    jmp disk_loop   ; if error jump to disk_loop

disk_loop:
    jmp $           ; infinite loop will let us know if there was an error
