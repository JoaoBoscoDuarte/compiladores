# Atividade 6 - EC2: Precedência e Associatividade

Compilador completo para a linguagem EC2 (evolução de EC1): expressões
aritméticas constantes, agora **sem exigir parênteses**, respeitando
precedência (`*`/`/` antes de `+`/`-`) e associatividade à esquerda.
Gramática (sem recursão à esquerda, ver `docs/`):

```
<exp_a> ::= <exp_m> (('+' | '-') <exp_m>)*
<exp_m> ::= <prim>  (('*' | '/') <prim>)*
<prim>  ::= <num> | '(' <exp_a> ')'
```

Parênteses continuam aceitos para forçar uma ordem diferente da padrão (ex.:
`(7 + 5) * 3`). A geração de código é a mesma da Atividade 5 (esquema de
pilha); só a análise sintática mudou.

## Compilar o compilador

```sh
make
```

Gera `bin/ec1`. `make clean` remove `bin/` e `obj/`.

## Executar

```sh
./bin/ec1 [--tokens] [--tree] [--eval] [--asm] <arquivo.ec1>
```

- `--tokens`: imprime a sequência de tokens.
- `--tree`: imprime a árvore sintática.
- `--eval`: interpreta e imprime o valor da expressão.
- `--asm`: gera o assembly em `asm/<nome>.s` (padrão se nenhuma opção for dada).

Para montar e executar o programa gerado (a partir desta pasta):

```sh
./bin/ec1 tests/ec1_precedencia.ec1
as --64 -o asm/ec1_precedencia.o asm/ec1_precedencia.s && ld -o asm/ec1_precedencia asm/ec1_precedencia.o
./asm/ec1_precedencia    # imprime 22 (7 + 5 * 3)
```

## Executar os testes

```sh
make && ./tests/run_ec1_tests.sh
```

Reaproveita os testes válidos/inválidos da Atividade 5 (parênteses
continuam funcionando) e acrescenta casos novos sem parênteses que
demonstram precedência (`7 + 5 * 3` = 22) e associatividade à esquerda
(`10 - 8 - 2` = 0, `100 / 10 / 2` = 5). Também gera e testa expressões
grandes e aleatórias. Requer `as` e `ld`.

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude) reescreveu o analisador sintático para a
gramática `exp_a`/`exp_m`/`prim` (precedência e associatividade, sem
recursão à esquerda), a partir do enunciado e do código da Atividade 5, e
adicionou os testes e esta documentação. O grupo revisou e testou o
resultado.
