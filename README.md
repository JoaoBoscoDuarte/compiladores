# Atividade 2 - Compiladores
> Compilador CI - x86-64 / Linux

Compilador simples: lê um arquivo `.ci` contendo um número, valida e gera o
assembly x86-64 correspondente (`.s`).

## Compilar o compilador

### Via Makefile

```sh
make
```

- Gera o binário em `bin/compilerci`. 
- `make clean` remove `bin/` e `obj/`.

### Manual via `g++`

```sh
mkdir bin && g++ src/*.cpp -o bin/compilerci
```

## Executar para um arquivo de entrada

```sh
./bin/compilerci tests/valid1.ci
```

- Gera o `.s` de saída em `asm/<nome>.s` (ex: `asm/valid1.s`).
- Para montar e linkar, execute `as`/`ld` a partir da raiz do projeto (o
  `.s` inclui `x86-64-linux/runtime.s` com caminho relativo à raiz, não à
  pasta `asm/`):
  ```sh
  as --64 -o asm/valid1.o asm/valid1.s && ld -o asm/valid1 asm/valid1.o
  ```

## Executar os testes

Os casos de teste estão em `tests/` (`valid1.ci` é válido, `invalid1.ci` e
`invalid2.ci` devem falhar na validação):

```sh
for f in tests/*.ci; do ./bin/compilerci "$f"; done
```

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude Code) foi utilizado como suporte para o
desenvolvimento do `Makefile`, organização da saída dos arquivos `.s` em uma
pasta única (`asm/`) e Gemini para auxílio no entendimento dos conceitos de montagem/
ligação do assembly x86-64. O código de análise e geração de código do
compilador foi desenvolvido e revisado pelo grupo.
