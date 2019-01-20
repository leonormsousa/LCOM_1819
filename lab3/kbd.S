#include<machine/asm.h>

#define OUT_BUF 0x60
#define STAT_REG 0x64
#define OBF 0x01
#define PAR_ERR 0x80
#define TO_ERR 0x40

.intel_syntax
.global _C_LABEL(scan_code)
.global _C_LABEL(status_error)
.global _C_LABEL(kbc_asm_ih)

.text
_C_LABEL(kbc_asm_ih):
inb STAT_REG
and eax, OBF
cmp eax, 0
je erro
inb OUT_BUF
mov scan_code, eax
inb STAT_REG
mov ecx, PAR_ERR
or ecx, TO_ERR
and eax, ecx
cmp eax, 0
jne fim
erro: mov al, 0
mov status_error, al
fim: ret
