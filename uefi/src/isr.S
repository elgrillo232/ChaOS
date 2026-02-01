.intel_syntax noprefix

.extern isr_common_handler

.macro ISR_NOERR n
  .global isr_stub_\n
  isr_stub_\n:
    push 0
    push \n
    jmp isr_common
.endm

.macro ISR_ERR n
  .global isr_stub_\n
  isr_stub_\n:
    push \n
    jmp isr_common
.endm

.global isr_stub_default
isr_stub_default:
  push 0
  push 255
  jmp isr_common

// Common layout at entry:
// [rsp+0]  vector
// [rsp+8]  error
// [rsp+16] rip
// [rsp+24] cs
// [rsp+32] rflags
isr_common:
  // Save general purpose registers.
  push rax
  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push rbp
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  // Windows x64 ABI requires 32-byte shadow space.
  sub rsp, 0x20

  mov rcx, rsp
  add rcx, 0x20
  call isr_common_handler

  add rsp, 0x20

  // If handler returns, restore regs and return from interrupt.
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rbp
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax

  add rsp, 16
  iretq

// Vectors 0-31
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR   30
ISR_NOERR 31
