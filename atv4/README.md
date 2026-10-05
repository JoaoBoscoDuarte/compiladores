# Atividade 4 - EC1: Análise Léxica e Sintática

Entrega desta atividade: o analisador léxico, sintático e o interpretador
da linguagem EC1 (`bin/ec1`), em [`../compiler`](../compiler). Enunciado em
`docs/`.

## Compilar

```sh
cd ../compiler && make
```

Gera `compiler/bin/ec1` (e também `compiler/bin/compilerci`, da atividade 2).

## Executar para um arquivo de entrada

```sh
cd ../compiler && ./bin/ec1 tests/ec1_valid2.ec1
```

Imprime a sequência de tokens, a árvore sintática e o valor da expressão
avaliada (interpretador). Erro léxico ou sintático reporta a posição do
erro e encerra com código de saída 1.

## Executar os testes

```sh
cd ../compiler && ./tests/run_ec1_tests.sh
```

Os casos estão em `compiler/tests/ec1_*.ec1`: dois programas válidos
(`ec1_valid1.ec1`, `ec1_valid2.ec1`) e dois com erro
(`ec1_invalid_lexico.ec1`, `ec1_invalid_sintatico.ec1`).

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude Code) implementou o analisador léxico, o
analisador sintático (descendente recursivo) e o interpretador da
linguagem EC1 a partir da gramática e do pseudocódigo do enunciado,
incluindo o formato de impressão de tokens e árvore. O grupo revisou o
código e validou a saída contra os exemplos do enunciado.
