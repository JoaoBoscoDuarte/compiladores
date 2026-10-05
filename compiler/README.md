# Compilador
> x86-64 / Linux

Compilador da disciplina, construído incrementalmente a cada atividade.
Dois programas, por enquanto:

- `compilerci` (Atividade 2) — linguagem CI (uma constante inteira), gera
  assembly x86-64.
- `ec1` (Atividade 4) — linguagem EC1 (expressões aritméticas constantes
  entre parênteses), faz análise léxica, análise sintática (árvore) e
  interpretação (avalia e imprime o resultado). Geração de assembly para
  EC1 fica para a próxima atividade.

## Compilar

### Via Makefile

```sh
make
```

- Gera `bin/compilerci` e `bin/ec1`.
- `make clean` remove `bin/` e `obj/`.

### Manual via `g++`

```sh
mkdir -p bin
g++ -Wall -Wextra -std=c++17 -Iinclude src/compilerci.cpp src/parser.cpp src/codegen.cpp -o bin/compilerci
g++ -Wall -Wextra -std=c++17 -Iinclude src/ec1.cpp src/ec1_lexer.cpp src/ec1_parser.cpp -o bin/ec1
```

## Executar

### compilerci (linguagem CI)

```sh
./bin/compilerci tests/valid1.ci
```

- Gera o `.s` de saída em `asm/<nome>.s` (ex: `asm/valid1.s`).
- Para montar e linkar, execute `as`/`ld` a partir da raiz deste diretório
  (`compiler/`) — o `.s` inclui `x86-64-linux/runtime.s` com caminho
  relativo à raiz, não à pasta `asm/`:
  ```sh
  as --64 -o asm/valid1.o asm/valid1.s && ld -o asm/valid1 asm/valid1.o
  ```

### ec1 (linguagem EC1)

```sh
./bin/ec1 tests/ec1_valid2.ec1
```

Imprime, em sequência, os tokens da análise léxica, a árvore sintática e o
valor da expressão interpretada. Erro léxico ou sintático termina a
execução (código de saída 1) com a posição do erro.

## Executar os testes

```sh
# compilerci: valid1.ci é válido, invalid1.ci e invalid2.ci devem falhar
for f in tests/*.ci; do ./bin/compilerci "$f"; done

# ec1: valida tokens/árvore/valor dos casos corretos e rejeita os com erro
./tests/run_ec1_tests.sh
```

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude Code) foi utilizado como suporte para o
desenvolvimento do `Makefile`, organização da saída dos arquivos `.s` em uma
pasta única (`asm/`), auxílio no entendimento dos conceitos de montagem/
ligação do assembly x86-64, e implementação do analisador léxico, sintático
(descendente recursivo) e interpretador da linguagem EC1 a partir do
pseudocódigo e da gramática do enunciado. O código foi revisado e testado
pelo grupo.
