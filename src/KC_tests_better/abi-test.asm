default rel

FAIL_LEN equ 16
SUCCES_LEN equ 17
SYS_WRITE equ 1
SYS_EXIT  equ 60
STDOUT    equ 1

section .rodata

big_cacl db "13432g3132g4g*53gY-g32ng+33g&123|33^gNZ~",0 ; wynik tego czegoś to podobno 18446744073709551359
message_fail db "Failed ABI test",10,0
message_succes db "ABI test success",10,0

section .text

global _start
global debug
extern notec

debug:
    mov     rax, rsp
    and     rax, 0xF
    cmp     rax, 8
    jne     fail
    mov     rdi, 420
    mov     rsi, 69
    mov     rdx, 2137
    mov     rcx, 2115
    mov     r8, 42069
    mov     r9, 2021
    xor     rax, rax
    ret

_start:
    push    1
    push    2
    push    3
    push    4
    mov     rbx, 1
    mov     rbp, 1
    mov     r12, 1
    mov     r13, 1
    mov     r14, 1
    mov     r15, 1
    mov     rdi, 0
    lea     rsi, [big_cacl]
    call    notec
    cmp     rax, 18446744073709551359
    jne     fail
    cmp     rbx, 1
    jne     fail
    cmp     rbp, 1
    jne     fail
    cmp     r12, 1
    jne     fail
    cmp     r13, 1
    jne     fail
    cmp     r14, 1
    jne     fail
    cmp     r15, 1
    jne     fail
    pop     rax
    cmp     rax, 4
    jne     fail
    pop     rax
    cmp     rax, 3
    jne     fail
    pop     rax
    cmp     rax, 2
    jne     fail
    pop     rax
    cmp     rax, 1
    jne     fail
    jmp     success


fail:
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [message_fail]
    mov     rdx, FAIL_LEN
    syscall
    mov     rax, SYS_EXIT
    mov     rdi, 0
    syscall

success:
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [message_succes]
    mov     rdx, SUCCES_LEN
    syscall
    mov     rax, SYS_EXIT
    mov     rdi, 0
    syscall
