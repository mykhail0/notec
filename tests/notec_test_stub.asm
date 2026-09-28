global notec_stub
global debug
extern notec
extern debug_stub
global base_saved_stack
global notec_stack
global debug_stack
extern broken_abi
[warning -reloc-rel-dword]

section .tbss
; thread save stack
align 8
base_saved_stack: resq 1

; notec base stack
align 8
notec_stack: resq 1

; debug base stack
align 8
debug_stack: resq 1

BROKEN_DEBUG_STACK equ 1
BROKEN_RBX equ 2
BROKEN_RBP equ 4
BROKEN_RSP equ 8
BROKEN_R12 equ 16
BROKEN_R13 equ 32
BROKEN_R14 equ 64
BROKEN_R15 equ 128

section .text
debug:
    add rsp, 8

    test rsp, 0xf
    jz  debug.good_stack
    or  DWORD [rel broken_abi], BROKEN_DEBUG_STACK
.good_stack:

    test rsi, 0x7
    jz  debug.good_stack_rsi
    or  DWORD [rel broken_abi], BROKEN_DEBUG_STACK
.good_stack_rsi:

    mov r8, r12         ; save r12 to r8
    mov r12, rsp        ; save rsp to r12

    mov r9, r13         ; save r13 to r9
    mov r13, [rsp - 8]  ; save return address to r13

    mov  r11, [rel debug_stack wrt ..gottpoff] ; get debug stack
    mov  rsp, [fs:r11]

    push r8             ; push initial values of r12 and r13
    push r9

    ; int64_t debug_stub(uint32_t n, uint64_t *stack_pointer, uint64_t **append_buffer_ptr);
    call debug_stub

    mov r9, r13         ; get return address and original rsp
    mov r8, r12
    pop r13             ; pop initial values of r13 and r12
    pop r12

    mov rsp, r8

    jmp r9
    ud2

section .rodata
; PRNG from https://nuclear.llnl.gov/CNP/rng/rngman/node4.html
align 8
RNG_MULTIPLIER: dq 0x27bb2ee687b0b0fd
RNG_ADDEND:     dq 0xb504f32d

%macro PRNG 1
    imul %1, [rel RNG_MULTIPLIER]
    add  %1, [rel RNG_ADDEND]
%endmacro

section .text

; overwrites rdi
; pops from stack and checks if count is equal to %1
; if not stores %2 to broken_abi
%macro pop_and_check 2
    pop  rdi
    cmp  rdi, %1
    je   %%check_ok
    or   DWORD [rel broken_abi], %2
%%check_ok:
%endmacro

notec_stub:
    sub rsp, 8

    mov  r11, [rel notec_stack wrt ..gottpoff] ; save notec stack
    mov  [fs:r11], rdx

    mov  r11, [rel debug_stack wrt ..gottpoff] ; save debug stack
    mov  [fs:r11], rcx

    ; save callee-saved registers
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    ; randomize registers
    PRNG rbp
    PRNG rbx
    PRNG r12
    PRNG r13
    PRNG r14
    PRNG r15

    ; save callee-saved registers again
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov  r11, [rel base_saved_stack wrt ..gottpoff] ; store stack
    mov  [fs:r11], rsp

    mov  rsp, rdx

    ; fill scratch registers with pseudo-random values
    PRNG rax
    PRNG rcx
    PRNG rdx
    PRNG r8
    PRNG r9
    PRNG r10
    PRNG r11

    call notec

    mov  r11, [rel notec_stack wrt ..gottpoff] ; get notec stack
    mov  rdx, [fs:r11]

    cmp  rdx, rsp
    je   notec_stub.rsp_is_ok
    or   DWORD [rel broken_abi], BROKEN_RSP
.rsp_is_ok:

    mov  r11, [rel base_saved_stack wrt ..gottpoff] ; get stack
    mov  rsp, [fs:r11]

    pop_and_check r15, BROKEN_R15
    pop_and_check r14, BROKEN_R14
    pop_and_check r13, BROKEN_R13
    pop_and_check r12, BROKEN_R12
    pop_and_check rbx, BROKEN_RBX
    pop_and_check rbp, BROKEN_RBP

    ; restore callee-saved registers
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  rbx
    pop  rbp

    add rsp, 8

    ret
    ud2
