# Atividade 5 - EC1: Geração de Código

Compilador completo para a linguagem EC1 (expressões aritméticas constantes
entre parênteses): análise léxica, análise sintática, interpretador e
gerador de assembly x86-64 / Linux. Enunciado em `docs/`.

A geração de código usa a pilha do sistema: o operando esquerdo é calculado
e empilhado (`push %rax`), o direito é calculado e movido para `%rbx`, o
esquerdo é desempilhado para `%rax` e a operação é feita (`add`, `sub`,
`imul`, ou `cqo` + `idiv`). Só `%rax`, `%rbx` (e `%rdx` na divisão) são usados,
e a pilha termina igual ao início, então funciona para expressões de
qualquer tamanho.

## Compilar o compilador

```sh
make
```

Gera `bin/ec1`. `make clean` remove `bin/` e `obj/`.
Sem Makefile: `mkdir -p bin && g++ -Wall -Wextra -std=c++17 -Iinclude src/*.cpp -o bin/ec1`

## Executar

```sh
./bin/ec1 [--tokens] [--tree] [--eval] [--asm] <arquivo.ec1>
```

- `--tokens`: imprime a sequência de tokens.
- `--tree`: imprime a árvore sintática.
- `--eval`: interpreta e imprime o valor da expressão.
- `--asm`: gera o assembly em `asm/<nome>.s` (padrão se nenhuma opção for dada).

As opções podem ser combinadas. Erros léxicos/sintáticos informam a posição
e encerram com código 1.

Para montar e executar o programa gerado (a partir desta pasta, pois o `.s`
inclui `x86-64-linux/runtime.s` com caminho relativo à raiz):

```sh
./bin/ec1 tests/ec1_valid2.ec1
as --64 -o asm/ec1_valid2.o asm/ec1_valid2.s && ld -o asm/ec1_valid2 asm/ec1_valid2.o
./asm/ec1_valid2    # imprime 10065
```

## Executar os testes

```sh
make && ./tests/run_ec1_tests.sh
```

Para cada programa válido, o script compila, monta, executa e compara o
valor impresso com o esperado e com o do interpretador. Também verifica a
rejeição de programas inválidos, expressões grandes (3000 operações
aninhadas) e 30 expressões aleatórias. Requer `as` e `ld`.

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude) implementou o gerador de código (esquema de
tradução com pilha), as opções de linha de comando do `ec1`, o script de
testes automáticos e esta documentação, a partir do enunciado e do código
das atividades anteriores. O grupo revisou e testou o resultado.
