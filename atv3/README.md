# Atividade 3 - Expressões Aritméticas em Assembly

Enunciado em `docs/`. Parte 1: um arquivo `.s` por expressão (`expr1.s`,
`expr2.s`, `expr3.s`), baseado no modelo `modelo.s`, cada um calculando o
resultado em `%rax` e imprimindo com `imprime_num`. Parte 2: respostas em
[`respostas.md`](respostas.md).

## Compilar (montar + linkar)

```sh
as --64 -o expr1.o expr1.s && ld -o expr1 expr1.o
as --64 -o expr2.o expr2.s && ld -o expr2 expr2.o
as --64 -o expr3.o expr3.s && ld -o expr3 expr3.o
```

## Executar

```sh
./expr1   # (19 * 15 - 10 * 7) + (117 - 33)      -> 299
./expr2   # (9 * 8 * 7) / (6 * 5 * 4 * 3 * 2)     -> 0
./expr3   # (42 - 222) * 11 + (19 * 88)           -> -308
```

## "Testes"

Não há suíte automatizada; a verificação é o próprio valor impresso por cada
executável, comparado à conta feita à mão (comentada em cada `.s`).

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude Code) foi utilizado para revisar a tradução das
três expressões para assembly x86-64 (registradores usados, `imul`/`idiv`,
sinal negativo) e para organizar a entrega em um arquivo por expressão,
conforme pedido no enunciado. As respostas da Parte 2 foram discutidas e
escritas com apoio do assistente.
