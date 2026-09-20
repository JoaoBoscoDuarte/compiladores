# Atividade 2 - Compilador CI

Entrega desta atividade: o compilador para a linguagem CI, em
[`../compiler`](../compiler). Enunciado em `docs/`.

## Compilar

```sh
cd ../compiler && make
```

Gera o binário em `compiler/bin/compilerci`.

## Executar para um arquivo de entrada

```sh
cd ../compiler && ./bin/compilerci tests/valid1.ci
```

Gera o `.s` em `compiler/asm/`. Ver [`../compiler/README.md`](../compiler/README.md)
para montar/linkar o `.s` gerado.

## Executar os testes

```sh
cd ../compiler && for f in tests/*.ci; do ./bin/compilerci "$f"; done
```

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude Code) foi utilizado como suporte para o
desenvolvimento do `Makefile`, organização da saída dos arquivos `.s` em uma
pasta única (`asm/`) e auxílio no entendimento dos conceitos de montagem/
ligação do assembly x86-64. O código de análise e geração de código do
compilador foi desenvolvido e revisado pelo grupo.
