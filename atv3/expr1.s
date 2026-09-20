  #
  # modelo de saida para o compilador
  #

  .section .text
  .globl _start

_start:
  ## expressao: (19 * 15 - 10 * 7) + (117 - 33)

  mov $19, %rax
  imul $15, %rax        # rax = 19 * 15 = 285

  mov $10, %rbx
  imul $7, %rbx          # rbx = 10 * 7 = 70
  sub %rbx, %rax         # rax = 285 - 70 = 215

  mov $117, %rcx
  sub $33, %rcx          # rcx = 117 - 33 = 84
  add %rcx, %rax         # rax = 215 + 84 = 299

  call imprime_num
  call sair

  .include "runtime.s"
