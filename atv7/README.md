# Atividade 7 - EV: Expressões com Variáveis

Compilador completo para a linguagem EV: zero ou mais declarações de
variável (`nome = exp;`), seguidas de uma expressão final (`= exp`) cujo
valor é o resultado do programa. Variáveis só podem usar outras já
declaradas antes delas (análise semântica com tabela de símbolos). A
geração de código reaproveita o esquema de pilha das atividades
anteriores; variáveis viram espaço reservado na seção `.bss`
(`.lcomm <nome>, 8`), lidas/escritas por nome em instruções `mov`.

## Compilar o compilador

```sh
make
```

Gera `bin/ev`. `make clean` remove `bin/` e `obj/`.

## Executar

```sh
./bin/ev [--tokens] [--tree] [--eval] [--asm] <arquivo.ev>
```

- `--tokens`: imprime a sequência de tokens.
- `--tree`: imprime as declarações e a expressão final.
- `--eval`: interpreta e imprime o valor da expressão final.
- `--asm`: gera o assembly em `asm/<nome>.s` (padrão se nenhuma opção for dada).

Erros léxicos, sintáticos e semânticos (variável não declarada) informam a
posição e encerram com código 1.

Para montar e executar o programa gerado (a partir desta pasta):

```sh
./bin/ev tests/ev_perimetro.ev
as --64 -o asm/ev_perimetro.o asm/ev_perimetro.s && ld -o asm/ev_perimetro asm/ev_perimetro.o
./asm/ev_perimetro    # imprime 140 (l=30; c=40; = l+l+c+c)
```

## Executar os testes

```sh
make && ./tests/run_ev_tests.sh
```

Cobre programas válidos com variáveis (exemplos do enunciado), rejeição
de variável não declarada (inclusive quando usada antes de sua própria
declaração), erro léxico de identificador inválido (`237axy`), e os
mesmos testes de expressões grandes/aleatórias das atividades
anteriores (sem variáveis). Requer `as` e `ld`.

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude) implementou o compilador completo da
linguagem EV (lexer, parser, análise semântica com tabela de símbolos,
geração de código com seção `.bss`), os testes e esta documentação, a
partir do enunciado e do código da Atividade 6. O grupo revisou e testou
o resultado.
