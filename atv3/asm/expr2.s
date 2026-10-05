  #
  # modelo de saida para o compilador
  #

  .section .text
  .globl _start

_start:
  ## expressao: (9 * 8 * 7) / (6 * 5 * 4 * 3 * 2)

  mov $9, %rax
  imul $8, %rax
  imul $7, %rax          # rax = 9 * 8 * 7 = 504

  mov $6, %rcx
  imul $5, %rcx
  imul $4, %rcx
  imul $3, %rcx
  imul $2, %rcx          # rcx = 6 * 5 * 4 * 3 * 2 = 720

  cqo
  idiv %rcx              # rax = 504 / 720 = 0

  call imprime_num
  call sair

  .include "runtime.s"
