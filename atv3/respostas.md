# Atividade 03 - Parte 2 - Respostas

**1. a1 + a2 + ... + an**

1 registrador. Basta um acumulador: `mov a1, rax` e depois `add a2, rax`, `add a3,
rax`, ... até `an`. Soma é associativa e cada termo é constante, não precisa
guardar nada além do acumulado.

**2. (a11*...*a1n) + ... + (am1*...*amn)**

2 registradores. Um acumulador para a soma final e um registrador auxiliar
para calcular cada produto da linha (mov + imul em sequência). Ao terminar
uma linha, soma o auxiliar no acumulador e reaproveita o auxiliar para a
próxima linha.

**3. Expressões constantes de tamanho arbitrário: registradores limitados ou
crescem sem limite?**

Sim, um número limitado de registradores sempre basta. Se a expressão tiver
mais resultados parciais pendentes do que registradores disponíveis, o
compilador guarda o excesso na pilha (`push`/`pop`) em vez de em registrador.
A pilha cresce conforme a memória disponível, então o número de
registradores usados pode ficar fixo (até 1 ou 2, como nos itens acima) e
quem escala com o tamanho da expressão é a pilha, não o conjunto de
registradores.
