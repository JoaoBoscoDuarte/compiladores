# Compilador
> x86-64 / Linux

Compilador da disciplina, construído incrementalmente a cada atividade.
`ev` (Atividades 4 a 7) — linguagem EV (expressões com variáveis):
declarações `nome = exp;` seguidas de uma expressão final `= exp`,
precedência/associatividade de operadores, análise léxica, sintática,
semântica (tabela de símbolos, variável precisa ser declarada antes de
usada) e geração de assembly x86-64.

A geração de código usa a pilha do sistema: o operando esquerdo é
calculado e empilhado (`push %rax`), o direito é calculado e movido para
`%rbx`, o esquerdo é desempilhado para `%rax` e a operação é feita (`add`,
`sub`, `imul`, ou `cqo` + `idiv`). Só `%rax`, `%rbx` (e `%rdx` na divisão)
são usados, e a pilha termina igual ao início, então funciona para
expressões de qualquer tamanho. Variáveis são reservadas na seção `.bss`
(`.lcomm <nome>, 8`) e lidas/escritas por nome em instruções `mov`.

## Compilar

### Via Makefile

```sh
make
```

- Gera `bin/ev`.
- `make clean` remove `bin/` e `obj/`.

### Manual via `g++`

```sh
mkdir -p bin
g++ -Wall -Wextra -std=c++17 -Iinclude src/ev.cpp src/ev_lexer.cpp src/ev_parser.cpp src/ev_semantic.cpp src/ev_codegen.cpp -o bin/ev
```

## Executar

```sh
./bin/ev [--tokens] [--tree] [--eval] [--asm] <arquivo.ev>
```

- `--tokens`: imprime a sequência de tokens.
- `--tree`: imprime as declarações e a expressão final.
- `--eval`: interpreta e imprime o valor da expressão final.
- `--asm`: gera o assembly em `asm/<nome>.s` (padrão se nenhuma opção for dada).

As opções podem ser combinadas. Erro léxico, sintático ou semântico
(variável não declarada) termina a execução (código de saída 1) com a
posição do erro.

Para montar e executar o programa gerado (a partir desta pasta, pois o `.s`
inclui `x86-64-linux/runtime.s` com caminho relativo à raiz):

```sh
./bin/ev tests/ev_perimetro.ev
as --64 -o asm/ev_perimetro.o asm/ev_perimetro.s && ld -o asm/ev_perimetro asm/ev_perimetro.o
./asm/ev_perimetro    # imprime 140
```

## Executar os testes

```sh
# compila, monta, roda e compara com o interpretador (requer as/ld)
./tests/run_ev_tests.sh
```

O script também gera e valida expressões grandes (3000 operações
aninhadas) e 30 expressões aleatórias (sem variáveis). Como a linguagem
não define o comportamento de divisão por zero, uma eventual
subexpressão aleatória que zere um divisor interno derruba o processo
(`idiv` por zero causa `SIGFPE` tanto no binário montado quanto no
interpretador) — o script detecta esses casos pelo código de saída do
interpretador e pula a amostra; isso é uma limitação conhecida da
geração aleatória de testes, não dos casos exigidos pelo enunciado (que
passam todos).

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude Code) foi utilizado como suporte para o
desenvolvimento do `Makefile`, organização da saída dos arquivos `.s` em uma
pasta única (`asm/`), auxílio no entendimento dos conceitos de montagem/
ligação do assembly x86-64, implementação do analisador léxico, sintático
(descendente recursivo), análise semântica (tabela de símbolos) e
interpretador da linguagem EV a partir do pseudocódigo e da gramática dos
enunciados, e do gerador de código (esquema de tradução com pilha mais
seção `.bss` para variáveis) e das opções de linha de comando do `ev`. O
código foi revisado e testado pelo grupo.
