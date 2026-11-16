#!/bin/sh
# Automatic tests for bin/cmd.
#  - valid programs: compile to assembly, assemble, link, run, and compare the
#    printed value with the expected one (and with the interpreter's value);
#  - invalid programs: the compiler must fail (exit 1) with the right kind of
#    error (lexical, syntactic or semantic);
#  - large and random programs are generated on the fly.
# Run from anywhere; requires `make` first plus `as` and `ld` (x86-64 Linux).
set -u

cd "$(dirname "$0")/.."
BIN="./bin/cmd"
OUT="asm"
mkdir -p "$OUT"

fail=0

# run_asm <file.cmd>: compiles, assembles, links and runs; prints the output.
run_asm() {
    name="$(basename "$1" .cmd)"
    "$BIN" "$1" >/dev/null || return 1
    as --64 -o "$OUT/$name.o" "$OUT/$name.s" || return 1
    ld -o "$OUT/$name" "$OUT/$name.o" || return 1
    timeout 20 "$OUT/$name"
}

check_valid() {
    file="$1"; expected="$2"
    got="$(run_asm "$file")"
    interp="$(timeout 20 "$BIN" --eval "$file" | tail -1)"
    if [ "$got" != "$expected" ]; then
        echo "FAIL $file: esperava '$expected', assembly imprimiu '$got'"
        fail=1
    elif [ "$interp" != "$expected" ]; then
        echo "FAIL $file: interpretador deu '$interp', esperava '$expected'"
        fail=1
    else
        echo "ok   $file -> $got"
    fi
}

# check_invalid <file> <lexico|sintatico|semantico>
check_invalid() {
    file="$1"; kind="$2"
    msg="$("$BIN" "$file" 2>&1 >/dev/null)"
    status=$?
    case "$msg" in
        "Erro $kind"*) ;;
        *) echo "FAIL $file: esperava erro $kind, obteve '$msg'"; fail=1; return ;;
    esac
    if [ $status -ne 1 ]; then
        echo "FAIL $file: codigo de saida $status (esperava 1)"
        fail=1
    else
        echo "ok   $file -> erro $kind rejeitado"
    fi
}

# Exemplos do enunciado.
check_valid tests/cmd_minimo.cmd 42
check_valid tests/cmd_delta.cmd 8
check_valid tests/cmd_soma.cmd 45
check_valid tests/cmd_resto.cmd 2
check_valid tests/cmd_mdc.cmd 6

# Comandos: if/else, while, atribuicao, aninhamento, blocos vazios.
check_valid tests/cmd_fatorial.cmd 3628800
check_valid tests/cmd_fibonacci.cmd 832040
check_valid tests/cmd_primos.cmd 25
check_valid tests/cmd_if_aninhado.cmd 2
check_valid tests/cmd_blocos_vazios.cmd 10
check_valid tests/cmd_while_zero_vezes.cmd 3
check_valid tests/cmd_while_if.cmd 90100
check_valid tests/cmd_atrib_propria.cmd 31
check_valid tests/cmd_negativos.cmd -1499
check_valid tests/cmd_nomes_runtime.cmd 193

# Comparacoes: valor 0/1 e precedencia menor que + e -.
check_valid tests/cmd_comparacoes.cmd 11001
check_valid tests/cmd_precedencia.cmd 1

# Erros lexicos.
check_invalid tests/cmd_invalid_lexico_ident.cmd lexico
check_invalid tests/cmd_invalid_lexico_char.cmd lexico

# Erros sintaticos.
check_invalid tests/cmd_invalid_sem_else.cmd sintatico
check_invalid tests/cmd_invalid_sem_chaves_if.cmd sintatico
check_invalid tests/cmd_invalid_sem_chaves_while.cmd sintatico
check_invalid tests/cmd_invalid_return_em_if.cmd sintatico
check_invalid tests/cmd_invalid_sem_return.cmd sintatico
check_invalid tests/cmd_invalid_sem_ponto_virgula.cmd sintatico
check_invalid tests/cmd_invalid_sem_corpo.cmd sintatico
check_invalid tests/cmd_invalid_lixo_final.cmd sintatico
check_invalid tests/cmd_invalid_igual_separado.cmd sintatico
check_invalid tests/cmd_invalid_palavra_chave_var.cmd sintatico
check_invalid tests/cmd_invalid_constante_grande.cmd sintatico

# Erros semanticos: variavel nao declarada (atribuicao, expressoes, condicoes,
# blocos aninhados, uso antes da declaracao).
check_invalid tests/cmd_invalid_atrib_nao_declarada.cmd semantico
check_invalid tests/cmd_invalid_rhs_nao_declarada.cmd semantico
check_invalid tests/cmd_invalid_ordem.cmd semantico
check_invalid tests/cmd_invalid_cond_if_nao_declarada.cmd semantico
check_invalid tests/cmd_invalid_cond_while_nao_declarada.cmd semantico
check_invalid tests/cmd_invalid_bloco_aninhado.cmd semantico
check_invalid tests/cmd_invalid_return_nao_declarada.cmd semantico

# Programas grandes (gerados).
N=3000
# expressao aninhada a esquerda / direita: N + 1
awk -v n=$N 'BEGIN { printf "{ return "; for (i=0;i<n;i++) printf "("; printf "1"; for (i=0;i<n;i++) printf " + 1)"; printf "; }\n" }' > "$OUT/cmd_grande_esq.cmd"
awk -v n=$N 'BEGIN { printf "{ return "; for (i=0;i<n;i++) printf "(1 + "; printf "1"; for (i=0;i<n;i++) printf ")"; printf "; }\n" }' > "$OUT/cmd_grande_dir.cmd"
check_valid "$OUT/cmd_grande_esq.cmd" $((N + 1))
check_valid "$OUT/cmd_grande_dir.cmd" $((N + 1))
# 2000 atribuicoes em sequencia
awk 'BEGIN { print "x = 0;\n{"; for (i=0;i<2000;i++) print "  x = x + 1;"; print "  return x;\n}" }' > "$OUT/cmd_muitas_atrib.cmd"
check_valid "$OUT/cmd_muitas_atrib.cmd" 2000
# 200 ifs aninhados (todos tomam o braco "then")
awk 'BEGIN { print "x = 0;\n{"; for (i=0;i<200;i++) print "if 1 {"; print "x = 7;"; for (i=0;i<200;i++) print "} else { x = 1; }"; print "return x;\n}" }' > "$OUT/cmd_if_fundo.cmd"
check_valid "$OUT/cmd_if_fundo.cmd" 7
# 200 whiles aninhados (cada um roda uma vez, via contadores)
awk 'BEGIN {
    for (i=0;i<50;i++) print "c" i " = 0;"
    print "t = 0;\n{"
    for (i=0;i<50;i++) print "while c" i " < 1 {\nc" i " = c" i " + 1;"
    print "t = t + 1;"
    for (i=0;i<50;i++) print "}"
    print "return t;\n}" }' > "$OUT/cmd_while_fundo.cmd"
check_valid "$OUT/cmd_while_fundo.cmd" 1
# loop longo: soma de 0 a N-1 = N(N-1)/2 (valores acima de 32 bits)
M=100000
awk -v m=$M 'BEGIN { print "i = 0;\ns = 0;\n{\nwhile i < " m " {\ns = s + i;\ni = i + 1;\n}\nreturn s;\n}" }' > "$OUT/cmd_loop_longo.cmd"
check_valid "$OUT/cmd_loop_longo.cmd" "$(awk -v m=$M 'BEGIN { printf "%d", m*(m-1)/2 }')"

# Programas aleatorios (sem divisao por variavel: o divisor e sempre uma
# constante nao nula): comandos if/while/atribuicao aninhados; o valor do
# assembly tem que bater com o do interpretador.
i=0
while [ $i -lt 40 ]; do
    awk -v seed=$i 'BEGIN {
        srand(seed + 1)
        for (v = 0; v < 4; v++) print "v" v " = " int(rand() * 20) ";"
        for (v = 0; v < 3; v++) print "k" v " = 0;"
        print "{"
        for (s = 0; s < 6; s++) stmt(0)
        print "return " expr(3) ";"
        print "}"
    }
    function var(   r) { r = int(rand() * 4); return "v" r }
    function expr(d,   r, op) {
        if (d == 0 || rand() < 0.25) return (rand() < 0.5) ? var() : int(rand() * 10)
        r = int(rand() * 6)
        if (r == 5) return "(" expr(d - 1) " / " (int(rand() * 9) + 1) ")"
        op = substr("+-*<>=", r + 1, 1)
        if (op == "=") op = "=="
        return "(" expr(d - 1) " " op " " expr(d - 1) ")"
    }
    function stmt(d,   r, k, n) {
        r = int(rand() * 3)
        if (d >= 3) r = 0
        if (r == 0) { print var() " = " expr(3) ";"; return }
        if (r == 1) {
            print "if " expr(2) " {"
            for (n = int(rand() * 3); n > 0; n--) stmt(d + 1)
            print "} else {"
            for (n = int(rand() * 3); n > 0; n--) stmt(d + 1)
            print "}"
            return
        }
        k = "k" d
        print k " = 0;"
        print "while " k " < " (int(rand() * 4) + 1) " {"
        for (n = int(rand() * 3); n > 0; n--) stmt(d + 1)
        print k " = " k " + 1;"
        print "}"
    }' > "$OUT/cmd_rand$i.cmd"
    expected="$(timeout 20 "$BIN" --eval "$OUT/cmd_rand$i.cmd" 2>/dev/null | tail -1)"
    if [ -z "$expected" ]; then
        echo "FAIL $OUT/cmd_rand$i.cmd: interpretador nao produziu valor"
        fail=1
    else
        check_valid "$OUT/cmd_rand$i.cmd" "$expected"
    fi
    i=$((i + 1))
done

if [ $fail -eq 0 ]; then echo "Todos os testes passaram."; else echo "HOUVE FALHAS."; fi
exit $fail
