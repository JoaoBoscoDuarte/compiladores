# Compilador
> x86-64 / Linux

Compilador da disciplina, construído incrementalmente a cada atividade.
`ec1` (Atividades 4 e 5) — linguagem EC1 (expressões aritméticas
constantes entre parênteses), completo: análise léxica, análise sintática
(árvore), interpretação e geração de assembly x86-64.

A geração de código usa a pilha do sistema: o operando esquerdo é
calculado e empilhado (`push %rax`), o direito é calculado e movido para
`%rbx`, o esquerdo é desempilhado para `%rax` e a operação é feita (`add`,
`sub`, `imul`, ou `cqo` + `idiv`). Só `%rax`, `%rbx` (e `%rdx` na divisão)
são usados, e a pilha termina igual ao início, então funciona para
expressões de qualquer tamanho.

## Compilar

### Via Makefile

```sh
make
```

- Gera `bin/ec1`.
- `make clean` remove `bin/` e `obj/`.

### Manual via `g++`

```sh
mkdir -p bin
g++ -Wall -Wextra -std=c++17 -Iinclude src/ec1.cpp src/ec1_lexer.cpp src/ec1_parser.cpp src/ec1_codegen.cpp -o bin/ec1
```

## Executar

```sh
./bin/ec1 [--tokens] [--tree] [--eval] [--asm] <arquivo.ec1>
```

- `--tokens`: imprime a sequência de tokens.
- `--tree`: imprime a árvore sintática.
- `--eval`: interpreta e imprime o valor da expressão.
- `--asm`: gera o assembly em `asm/<nome>.s` (padrão se nenhuma opção for dada).

As opções podem ser combinadas. Erro léxico ou sintático termina a
execução (código de saída 1) com a posição do erro.

Para montar e executar o programa gerado (a partir desta pasta, pois o `.s`
inclui `x86-64-linux/runtime.s` com caminho relativo à raiz):

```sh
./bin/ec1 tests/ec1_valid2.ec1
as --64 -o asm/ec1_valid2.o asm/ec1_valid2.s && ld -o asm/ec1_valid2 asm/ec1_valid2.o
./asm/ec1_valid2    # imprime 10065
```

## Executar os testes

```sh
# compila, monta, roda e compara com o interpretador (requer as/ld)
./tests/run_ec1_tests.sh
```

O script de testes do `ec1` também gera e valida expressões grandes (3000
operações aninhadas) e 30 expressões aleatórias. Como a linguagem EC1 não
define o comportamento de divisão por zero, uma eventual subexpressão
aleatória que zere um divisor interno derruba o processo (`idiv` por zero
causa `SIGFPE` tanto no binário montado quanto no interpretador) — isso é
uma limitação conhecida da geração aleatória de testes, não dos casos
exigidos pelo enunciado (que passam todos).

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude Code) foi utilizado como suporte para o
desenvolvimento do `Makefile`, organização da saída dos arquivos `.s` em uma
pasta única (`asm/`), auxílio no entendimento dos conceitos de montagem/
ligação do assembly x86-64, implementação do analisador léxico, sintático
(descendente recursivo) e interpretador da linguagem EC1 a partir do
pseudocódigo e da gramática do enunciado, e do gerador de código (esquema
de tradução com pilha) e das opções de linha de comando do `ec1` a partir
do enunciado da Atividade 5. O código foi revisado e testado pelo grupo.
