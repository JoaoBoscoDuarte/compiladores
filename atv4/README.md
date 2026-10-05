# Atividade 4 - EC1: Análise Léxica e Sintática

Analisador léxico, sintático (descendente recursivo) e interpretador para a
linguagem EC1 (expressões aritméticas constantes entre parênteses).
Enunciado em `docs/`.

## Compilar

### Via Makefile

```sh
make
```

- Gera o binário em `bin/ec1`.
- `make clean` remove `bin/` e `obj/`.

### Manual via `g++`

```sh
mkdir -p bin && g++ -Wall -Wextra -std=c++17 -Iinclude src/*.cpp -o bin/ec1
```

## Executar para um arquivo de entrada

```sh
./bin/ec1 tests/valid2.ec1
```

Imprime, em sequência: os tokens da análise léxica, a árvore sintática e o
valor da expressão interpretada. Erro léxico ou sintático reporta a posição
do erro e encerra com código de saída 1.

## Executar os testes

```sh
./tests/run_tests.sh
```

Casos em `tests/`: `valid1.ec1` e `valid2.ec1` são válidos;
`invalid_lexico.ec1` e `invalid_sintatico.ec1` devem falhar.

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
