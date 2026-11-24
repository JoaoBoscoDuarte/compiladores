# Atividade 9 - Fun: Funções

Compilador para a linguagem Fun (Cmd + funções), conforme o enunciado:
declarações de variáveis globais (`var nome = exp;`) e de funções
(`fun nome(params) { var ...; comandos; return exp; }`), seguidas do bloco
`main { comandos; return exp; }`. Chamadas de função são expressões
(`f(a, b)`); funções diretamente recursivas são aceitas, recursão mútua não
(declaração anterior ao uso, como no guia). Parâmetros e variáveis locais
escondem globais de mesmo nome.

Geração de código: convenção do enunciado (argumentos empilhados do último
ao primeiro, `call`, chamador remove os argumentos; função com `push %rbp`,
espaço das locais, locais em `8*i(%rbp)` e parâmetros em
`8*L+16+8*j(%rbp)`; resultado em `%rax`). Variáveis globais ficam na `.bss`.

Diferenças em relação ao guia:
- rótulos de função e globais saem com prefixo (`fun_<nome>`, `var_<nome>`),
  para não colidirem com rótulos e símbolos do runtime;
- vírgula sobrando em listas de parâmetros/argumentos é erro sintático;
- redeclarar um nome no mesmo escopo (global, função, parâmetro, local) é
  erro semântico;
- ordem de avaliação definida pelo código gerado: argumentos da direita para
  a esquerda, e em `<`, `>`, `==` o lado direito primeiro (o interpretador
  segue a mesma ordem; só importa quando funções alteram globais).

## Compilar o compilador

```sh
make
```

Gera `bin/fun`. `make clean` remove `bin/` e `obj/`.

## Executar

```sh
./bin/fun [--tokens] [--tree] [--eval] [--asm] <arquivo.fun>
```

- `--tokens`: sequência de tokens.
- `--tree`: globais, funções e bloco `main`.
- `--eval`: executa o programa no interpretador e imprime o valor retornado.
- `--asm`: gera o assembly em `asm/<nome>.s` (padrão se nenhuma opção for dada).

Erros léxicos, sintáticos e semânticos informam a posição e encerram com
código 1. Para montar e executar o programa gerado (a partir desta pasta):

```sh
./bin/fun tests/fun_fib.fun
as --64 -o asm/fun_fib.o asm/fun_fib.s && ld -o asm/fun_fib asm/fun_fib.o
./asm/fun_fib    # imprime 89
```

## Executar os testes

```sh
make && ./tests/run_fun_tests.sh
```

Compila, monta, executa e compara o resultado do assembly com o valor
esperado e com o do interpretador (requer `as` e `ld`). Cobre os exemplos do
enunciado, funções recursivas (fibonacci, fatorial, mdc, Ackermann, recursão
profunda), funções que chamam outras, com e sem parâmetros e variáveis
locais, escopo e globais, rejeição de erros léxicos, sintáticos e semânticos
(função/variável não declarada, aridade, recursão mútua, redeclaração),
programas grandes gerados e 40 programas aleatórios com funções.

## Grupo
- GUILHERME URTIGA LOPES SILVA
- JOÃO BOSCO DUARTE FILHO
- JOÃO VICTOR OLIVEIRA SANTOS

## Uso de LLM

O assistente de IA (Claude Code) implementou a extensão do compilador Cmd
para Fun (novos tokens e palavras-chave, parser de declarações e chamadas,
análise semântica com tabelas global e local, geração de código com
registros de ativação, interpretador), os testes e esta documentação, a
partir do enunciado e do código da Atividade 8. O grupo revisou e testou o
resultado.
