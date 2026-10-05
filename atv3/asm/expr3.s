  #
  # modelo de saida para o compilador
  #

  .section .text
  .globl _start

_start:
  ## expressao: (42 - 222) * 11 + (19 * 88)

  mov $42, %rax
  sub $222, %rax         # rax = 42 - 222 = -180
  imul $11, %rax         # rax = -180 * 11 = -1980

  mov $19, %rbx
  imul $88, %rbx         # rbx = 19 * 88 = 1672

  add %rbx, %rax         # rax = -1980 + 1672 = -308

  call imprime_num
  call sair

  .include "runtime.s"
