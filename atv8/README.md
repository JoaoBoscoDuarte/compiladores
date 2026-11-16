# Atividade 8 - Cmd: Comandos

Compilador para a linguagem Cmd (EV + comandos), conforme o enunciado, sem
alterações na sintaxe: declarações `nome = exp;`, um corpo `{ ... }` com
comandos (`if`/`else` com `else` obrigatório, `while`, atribuição) e
`return exp;`. Operadores `<`, `>` e `==` (precedência menor que `+`/`-`)
resultam em 1 ou 0; zero é falso e qualquer outro valor é verdadeiro.
Atribuir ou usar variável não declarada é erro semântico.

Geração de código: modelos do enunciado (`cmp`/`setl`/`setg`/`setz`,
`jz`/`jmp` com rótulos `Lfalso<n>`, `Lfim<n>`, `Linicio<n>`). Variáveis ficam
na `.bss` com prefixo `var_` (evita conflito com rótulos e símbolos do
runtime). Divisão por zero não tem comportamento definido (como em EV).

## Compilar o compilador

```sh
make
```

Gera `bin/cmd`. `make clean` remove `bin/` e `obj/`.

## Executar

```sh
./bin/cmd [--tokens] [--tree] [--eval] [--asm] <arquivo.cmd>
```

- `--tokens`: sequência de tokens.
- `--tree`: declarações, comandos (indentados) e expressão de retorno.
- `--eval`: executa o programa no interpretador e imprime o valor retornado.
- `--asm`: gera o assembly em `asm/<nome>.s` (padrão se nenhuma opção for dada).

Erros léxicos, sintáticos e semânticos informam a posição e encerram com
código 1. Para montar e executar o programa gerado (a partir desta pasta):

```sh
./bin/cmd tests/cmd_mdc.cmd
as --64 -o asm/cmd_mdc.o asm/cmd_mdc.s && ld -o asm/cmd_mdc asm/cmd_mdc.o
./asm/cmd_mdc    # imprime 6
```

## Executar os testes

```sh
make && ./tests/run_cmd_tests.sh
```

Compila, monta, executa e compara o resultado do assembly com o valor
esperado e com o do interpretador (requer `as` e `ld`). Cobre os exemplos do
enunciado, `if`/`while`/atribuição (aninhados, blocos vazios, laços longos),
comparações e precedência, rejeição de erros léxicos, sintáticos e semânticos,
programas grandes gerados e 40 programas aleatórios com comandos.

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude Code) implementou a extensão do compilador EV
para Cmd (novos tokens e palavras-chave, parser de comandos e comparações,
análise semântica de atribuições, geração de código com rótulos e saltos,
interpretador), os testes e esta documentação, a partir do enunciado e do
código da Atividade 7. O grupo revisou e testou o resultado.
