BITS 32

global kos_setegid
global kos_seteuid
global kos_setgroups

section .text

kos_setegid:
    mov eax, 71
    mov ebx, edi
    mov ecx, edi
    int 0x80
    ret

kos_seteuid:
    mov eax, 70
    mov ebx, edi
    mov ecx, edi
    int 0x80
    ret

kos_setgroups:
    mov eax, 206
    mov ebx, edi
    mov ecx, esi
    int 0x80
    ret
