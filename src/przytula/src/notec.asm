default rel

extern debug

SYS_EXIT equ 60
NOT_WAIT equ 0
NOT_READ equ 0
READ equ 1

; Transitions to usual mode.
%define usual_mode xor r15b, r15b

section .bss

align 8
; Noteć to wait for would have a number one more than in reality,
; because the array is initialized to 0 (-1 would be perfect).
waiting_for resq N
; The top of the stack to be swapped.
align 8
stack_top resq N

section .text

global notec

; uint64_t notec(uint32_t n, char const *calc);
; %rdi -> n
; %rsi -> calc
; Return value in %rax
notec:
  ; Preserve caller registers.
  mov rax, rsp
  mov rcx, rbp
  push rbx
  push rcx
  push r12
  push r13
  push r14
  push r15
  push rax
  ; r14 stores the address on the stack where caller registers are preserved.
  mov r14, rax
  sub r14, 8

  lea r13, [rsi] ; saving string address
  mov rbx, rdi ; saving noteć id
  ; The mode of the notec, 0 is usual mode,
  ; 1 is reading mode for inputting the number on top of the stack.
  xor r15d, r15d
  jmp str_ptr_increased
next_calculation:
  inc r13
str_ptr_increased:
  mov al, byte [r13]
  ; If the string to be calculated is processed, the thread can exit.
  test al, al
  jz exit
  cmp al, 97 ; 'a'
  jge more_than_a
  cmp al, 65 ; 'A'
  jge more_than_A
  cmp al, 48 ; '0'
  jge more_than_0

  usual_mode
  cmp al, 38 ; '&'
  je log_and
  cmp al, 42 ; '*'
  je multiply
  cmp al, 43 ; '+'
  je sum
  cmp al, 45 ; '-'
  je make_negative
  jmp error

more_than_a:
  cmp al, 102 ; 'f'
  jle read_num_a
  usual_mode
  cmp al, 103 ; 'g'
  je call_debug
  cmp al, 110 ; 'n'
  je push_id
  cmp al, 124 ; '|'
  je log_or
  cmp al, 126 ; '~'
  je log_not
  jmp error

more_than_A:
  cmp al, 70 ; 'F'
  jle read_num_A
  usual_mode
  cmp al, 78 ; 'N'
  je push_notec
  cmp al, 87 ; 'W'
  je synchronize
  cmp al, 88 ; 'X'
  je switch
  cmp al, 89 ; 'Y'
  je duplicate
  cmp al, 90 ; 'Z'
  je remove
  cmp al, 94 ; '^'
  je log_xor
  jmp error

more_than_0:
  cmp al, 57 ; '9'
  jle read_num_0
  usual_mode
  cmp al, 61 ; '='
  jne error
  jmp next_calculation

read_num_a:
  sub al, 87 ; - 'a' + 10
  jmp read_num
read_num_A:
  sub al, 55 ; - 'A' + 10
  jmp read_num
read_num_0:
  sub al, 48
  jmp read_num

; POSSIBLE NOTEĆ CALCULATIONS
; Every operation is performed modulo 2^64.

; Reads a number in 16 base to the top of the stack.
read_num:
  ; If in usual mode, read_num should not pop the value from the stack.
  test r15b, r15b
  jz entering_read_mode
  pop r12
  jmp r12_assigned
entering_read_mode:
  xor r12d, r12d
  inc r15b ; r15b = 1
r12_assigned:
  mov cl, al
  mov rax, 16
  mul r12
  add al, cl
  push rax
  jmp next_calculation

; Pops two values from the stack and pushes their sum.
sum:
  pop r12
  pop r8
  add r12, r8
  push r12
  jmp next_calculation

; Pops two values from the stack and pushes their product.
multiply:
  pop rax
  pop r12
  mul r12
  push rax
  jmp next_calculation

; Pops the value from the stack and pushes its negative.
make_negative:
  pop r12
  neg r12
  push r12
  jmp next_calculation

; Pops two values from the stack and pushes their bitwise and.
log_and:
  pop r12
  pop r8
  and r12, r8
  push r12
  jmp next_calculation

; Pops two values from the stack and pushes their bitwise or.
log_or:
  pop r12
  pop r8
  or r12, r8
  push r12
  jmp next_calculation

; Pops two values from the stack and pushes their bitwise xor.
log_xor:
  pop r12
  pop r8
  xor r12, r8
  push r12
  jmp next_calculation

; Pops the value from the stack and pushes it negated bitwise.
log_not:
  pop r12
  not r12
  push r12
  jmp next_calculation

; Pops the value from the stack.
remove:
  pop r12
  jmp next_calculation

; Duplicates the value on top of the stack.
duplicate:
  pop r12
  push r12
  push r12
  jmp next_calculation

; Switches two values on top of the stack.
switch:
  pop r12
  pop r8
  xchg r12, r8
  push r8
  push r12
  jmp next_calculation

; Pushes my noteć number.
push_id:
  push rbx
  jmp next_calculation

; Pushes noteć count.
push_notec:
  push N
  jmp next_calculation

; Calls the debug function of
; int64_t debug(uint32_t n, uint64_t *stack_pointer);
call_debug:
  mov rdi, rbx
  mov rsi, rsp
  mov r12, rsp
  and rsp, -16 ; Aligning the stack pointer to conform ABI.
  call debug
  lea rsp, [r12 + 8 * rax] ; The adjustment of the stack pointer.
  jmp next_calculation

; Pop the value m from the stack, treat it as noteć id. Wait until noteć m receives 'W'
; and pops my id from its stack. Then switch values from the stack tops.
synchronize:
  pop r8 ; Noteć number to swap with and to wait for.
  pop r9 ; Value to swap.

  lea rcx, [stack_top]
  mov qword [rcx + 8 * rbx], r9

  lea rdx, [waiting_for]
  inc r8
  mov qword [rdx + 8 * rbx], r8
  dec r8

; Wait until noteć I am waiting for isn't waiting back for me
; (which means, stack_top is assigned alright).
wait_to_read:
  mov r10, qword [rdx + 8 * r8]
  dec r10
  cmp rbx, r10
  jne wait_to_read
  ; Get the value from the stack_top of the partner.
  push qword [rcx + 8 * r8]
  ; Let the partner know that you got the value.
  mov qword [rdx + 8 * r8], NOT_WAIT

; Now wait until the partner gets the value from my stack_top.
wait_to_leave:
  mov r10, qword [rdx + rbx]
  cmp r10, NOT_WAIT
  jne wait_to_leave

  jmp next_calculation

; Returns rsp in rax before notec was called and rbp in rdx
exit_procedure:
  ; Restore caller registers.
  mov rax, r14
  mov rbx, qword [rax]
  mov rdx, qword [rax - 8]
  mov r12, qword [rax - 16]
  mov r13, qword [rax - 24]
  mov r14, qword [rax - 32]
  mov r15, qword [rax - 40]
  mov rax, qword [rax - 48]
  ret

error:
  call exit_procedure
  mov rsp, rax
  mov rbp, rdx
  mov eax, SYS_EXIT
  mov edi, 1
  syscall
  ret

exit:
  call exit_procedure
  mov rcx, rax
  pop rax
  mov rsp, rcx
  mov rbp, rdx
  ret
