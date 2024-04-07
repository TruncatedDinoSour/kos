BITS 64

global kos_setegid
global kos_seteuid
global kos_setgroups

section .text

kos_setegid:
    mov rax, 114
    mov rdi, rdi
    mov rsi, rdi
    syscall
    ret

kos_seteuid:
    mov rax, 113
    mov rdi, rdi
    mov rsi, rdi
    syscall
    ret

kos_setgroups:
    mov rax, 116
    mov rdi, rdi
    mov rsi, rsi
    syscall
    ret
