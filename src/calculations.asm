section .text
global iterate
extern log
extern pow
; args are in rdi (z), rsi (c), rdx (itLeft)
iterate:
    mov r9, rdx ; this will be the total number of iterations
    mov r10, rdx ; r10 will be the one we dec
loop:
    ; square
    mov r8, QWORD [rdi] ; a (first 8 bytes of object)

    fld QWORD [rdi]
    fld QWORD [rdi]
    ; okay so this is actually a huge brain manuver - i put a on
    ; the stack 2x so that i have the original value of a later
    fld QWORD [rdi + 8]
    faddp ; top of stack is now a + b
    
    fld QWORD [rdi]
    fld QWORD [rdi + 8]
    fsubp
    ; stack is now a + b | a - b
    fmulp
    fstp QWORD [rdi]
    ; mov QWORD [rdi], rax ; a is now = (a + b) * (a - b)

    fld1
    fld1
    faddp ; puts 2 on the top of the stack (good?)
    fmulp ; original a was on the stack from before, we just put 2 on there
    fld QWORD [rdi + 8] ; b
    fmulp
    fstp QWORD [rdi + 8] ; 2 * a * b
    ; done squaring

    ; z += c
    fld QWORD [rsi] ; c_a
    fld QWORD [rdi] ; z_a
    faddp
    fstp QWORD [rdi]

    fld QWORD [rsi + 8] ; c_b
    fld QWORD [rdi + 8] ; z_b
    faddp
    fstp QWORD [rdi + 8]

    ; squared modulus
    fld QWORD [rdi]
    fld QWORD [rdi]
    fmulp ; top stack is now a * a
    fld QWORD [rdi + 8]
    fld QWORD [rdi + 8]
    fmulp ; top stack is now b * b
    faddp ; top stack is now a * a + b * b
    ; mov rax, QWORD [rdi] ; store a value
    ; fstp QWORD [rdi] ; get R
    ; mov rbx, QWORD [rdi] ; store R in register
    ; mov QWORD [rdi], rax ; put a back

    mov rax, QWORD [rdi] ; store a value
    mov QWORD [rdi], 9
    fild DWORD [rdi] ; put 9 on top of stack
    mov QWORD [rdi], rax ; put a back
    fcomp ; pops the 9, R is on top
    ; okay so if 9 (st0) < R (st1), c0 is set
    ; c0 is bit 8 (0 indexed, from right) of FPU control
    ; word (fctrl in gdb)
    fnstsw ax ; copy flags to AX (fctrl)
    and ax, 0b0000000100000000 ; get bit 8 (c0) only
    cmp ax, 0 ; if c0 is unset (9 >= R), will be 0
    jne finish ; 9 is less than radius
    jmp continue

finish:
    mov rax, QWORD [rdi] ; store a value
    fstp QWORD [rdi] ; get R value
    mov rbx, QWORD [rdi] ; R value in register
    mov QWORD [rdi], rax ; put a back
    mov r11, rdi ; save z
    mov r12, rsi ; save c (necessary?)
    mov rdi, rbx ; rdi is first arg
    xor rbx, rbx ; dont know if this is necessary
    call log WRT ..plt ; rax should be log(R) now
    mov r8, rax ; save it in some random register
    mov rbx, r9
    sub rbx, rdx ; rbx is now numIterations - itLeft
    mov rdi, 2
    mov rsi, rbx
    call pow WRT ..plt
    mov rdi, r11 ; get back z
    mov r11, QWORD [rdi] ; save a value
    mov QWORD [rdi], rax ; put power in num
    fld QWORD [rdi]
    mov QWORD [rdi], r8 ; log(R) from before
    fld QWORD [rdi]
    mov QWORD [rdi], r11 ; put a back
    fdivrp ; divides log(R) by the power
    ret

continue:
    ; get rid of extra number from top of stack
    mov rax, QWORD [rdi]
    fstp QWORD [rdi]
    mov QWORD [rdi], rax

    dec r10

    cmp r10, 0
    jg loop

    ; return 0
    mov rax, 0
    ret