#!/bin/sh
# Automatic tests for bin/fun.
#  - valid programs: compile to assembly, assemble, link, run, and compare the
#    printed value with the expected one (and with the interpreter's value);
#  - invalid programs: the compiler must fail (exit 1) with the right kind of
#    error (lexical, syntactic or semantic);
#  - large and random programs are generated on the fly.
# Run from anywhere; requires `make` first plus `as` and `ld` (x86-64 Linux).
set -u

cd "$(dirname "$0")/.."
BIN="./bin/fun"
OUT="asm"
mkdir -p "$OUT"

fail=0

# run_asm <file.fun>: compiles, assembles, links and runs; prints the output.
run_asm() {
    name="$(basename "$1" .fun)"
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

# Exemplos do enunciado: abs, fib recursiva, f(x) com variaveis locais.
check_valid tests/fun_minimo.fun 42
check_valid tests/fun_abs.fun 1705
check_valid tests/fun_fib.fun 89
check_valid tests/fun_locais_pdf.fun 131714583
check_valid tests/fun_ordem_params.fun 10798

# Funcoes com/sem parametros, com/sem variaveis locais.
check_valid tests/fun_sem_params.fun 20
check_valid tests/fun_sem_locais.fun 321
check_valid tests/fun_so_retorno.fun 5
check_valid tests/fun_muitos_params.fun 204

# Funcoes que chamam outras e chamadas dentro de expressoes.
check_valid tests/fun_cadeia.fun 168
check_valid tests/fun_args_chamadas.fun 1214
check_valid tests/fun_chamada_cond.fun 90
check_valid tests/fun_while_func.fun 1015015
check_valid tests/fun_init_global_local.fun 491260
check_valid tests/fun_recursao_local_cmds.fun 3025

# Recursao.
check_valid tests/fun_fatorial.fun 1307674368000
check_valid tests/fun_mdc.fun 21006
check_valid tests/fun_ackermann.fun 961
check_valid tests/fun_recursao_profunda.fun 4501500

# Escopo, variaveis globais e nomes iguais a simbolos do runtime.
check_valid tests/fun_esconde_global.fun 60100900
check_valid tests/fun_altera_global.fun 707
check_valid tests/fun_nomes_runtime.fun 1302
# Ordem de avaliacao (argumentos da direita p/ esquerda; comparacao: direita antes).
check_valid tests/fun_ordem_avaliacao.fun 32423022

# Erros lexicos.
check_invalid tests/fun_invalid_lexico.fun lexico

# Erros sintaticos.
for t in decl_sem_var sem_main main_sem_chaves var_em_main funcao_sem_chaves funcao_sem_parenteses \
         funcao_sem_return funcao_aninhada return_em_if var_apos_comando virgula_sobrando \
         virgula_sobrando_chamada virgula_dupla param_nao_id chamada_sem_fecha \
         palavra_chave_var palavra_chave_param chamada_comando lixo_final; do
    check_invalid "tests/fun_invalid_$t.fun" sintatico
done

# Erros semanticos: nomes (variavel/funcao nao declarada, escopo), aridade,
# tipo de entidade (variavel x funcao), recursao mutua e redeclaracoes.
for t in funcao_nao_declarada var_nao_declarada arg_nao_declarado declarada_depois \
         aridade_menos aridade_mais aridade_zero chamar_variavel chamar_local \
         funcao_como_variavel atribuir_funcao recursao_mutua \
         redecl_funcao redecl_global redecl_var_funcao param_repetido local_repetida local_igual_param \
         local_fora_da_funcao local_de_outra_funcao param_fora local_usada_antes global_depois \
         global_usa_propria local_usa_propria main_atrib_local_alheia; do
    check_invalid "tests/fun_invalid_$t.fun" semantico
done

# Programas grandes (gerados).
N=3000
# expressao aninhada a esquerda / direita: N + 1
awk -v n=$N 'BEGIN { printf "main { return "; for (i=0;i<n;i++) printf "("; printf "1"; for (i=0;i<n;i++) printf " + 1)"; printf "; }\n" }' > "$OUT/fun_grande_esq.fun"
awk -v n=$N 'BEGIN { printf "main { return "; for (i=0;i<n;i++) printf "(1 + "; printf "1"; for (i=0;i<n;i++) printf ")"; printf "; }\n" }' > "$OUT/fun_grande_dir.fun"
check_valid "$OUT/fun_grande_esq.fun" $((N + 1))
check_valid "$OUT/fun_grande_dir.fun" $((N + 1))
# 500 funcoes encadeadas: f0(x) = x + 1, fi(x) = f(i-1)(x) + 1  ->  f499(0) = 500
awk 'BEGIN { print "fun f0(x) { return x + 1; }"
    for (i=1;i<500;i++) print "fun f" i "(x) { return f" (i-1) "(x) + 1; }"
    print "main { return f499(0); }" }' > "$OUT/fun_cadeia_500.fun"
check_valid "$OUT/fun_cadeia_500.fun" 500
# chamada aninhada inc(inc(...inc(0)...)) com 500 niveis
awk 'BEGIN { print "fun inc(x) { return x + 1; }"; printf "main { return "
    for (i=0;i<500;i++) printf "inc("; printf "0"; for (i=0;i<500;i++) printf ")"; print "; }" }' > "$OUT/fun_chamada_aninhada.fun"
check_valid "$OUT/fun_chamada_aninhada.fun" 500
# funcao com 100 variaveis locais: a_i = a_(i-1) + 1, retorna a99 -> x + 99
awk 'BEGIN { print "fun f(x) {"; print "var a0 = x;"
    for (i=1;i<100;i++) print "var a" i " = a" (i-1) " + 1;"
    print "return a99; }"; print "main { return f(1); }" }' > "$OUT/fun_100_locais.fun"
check_valid "$OUT/fun_100_locais.fun" 100
# funcao com 50 parametros: soma de p_i * (i+1) chamada com p_i = i+1 -> soma dos quadrados 1..50
awk 'BEGIN { printf "fun f("; for (i=0;i<50;i++) printf "%sp%d", (i?", ":""), i; print ") {"
    printf "return "; for (i=0;i<50;i++) printf "%sp%d * %d", (i?" + ":""), i, i+1; print "; }"
    printf "main { return f("; for (i=0;i<50;i++) printf "%s%d", (i?", ":""), i+1; print "); }" }' > "$OUT/fun_50_params.fun"
check_valid "$OUT/fun_50_params.fun" 42925

# Programas aleatorios: funcoes (aridade 0 a 3, com locais, chamando funcoes
# anteriores e as vezes alterando globais) + main com if/while/atribuicao e
# chamadas em expressoes (divisor sempre constante nao nula). O valor do
# assembly tem que bater com o do interpretador.
i=0
while [ $i -lt 40 ]; do
    awk -v seed=$i 'BEGIN {
        srand(seed + 1)
        for (v = 0; v < 4; v++) print "var v" v " = " int(rand() * 20) ";"
        for (v = 0; v < 3; v++) print "var k" v " = 0;"
        nf = 5
        for (f = 0; f < nf; f++) {
            ar[f] = int(rand() * 4)
            printf "fun f%d(", f
            for (p = 0; p < ar[f]; p++) printf "%sp%d", (p ? ", " : ""), p
            print ") {"
            nl = int(rand() * 3)
            nloc = 0
            for (l = 0; l < nl; l++) {
                printf "var l%d = ", l
                cur = f; ctxp = ar[f]; ctxl = nloc
                print expr(2) ";"
                nloc++
            }
            cur = f; ctxp = ar[f]; ctxl = nloc
            for (s = int(rand() * 3); s > 0; s--) fstmt()
            print "return " expr(3) ";"
            print "}"
        }
        cur = nf; ctxp = 0; ctxl = 0
        print "main {"
        for (s = 0; s < 6; s++) stmt(0)
        # the result mixes every global, so any wrong side effect shows up
        print "return v0 + v1 * 3 + v2 * 5 + v3 * 7 + " expr(3) ";"
        print "}"
    }
    # variable leaf: global, local or parameter of the current function
    function leaf(   r) {
        r = rand()
        if (ctxp > 0 && r < 0.3) return "p" int(rand() * ctxp)
        if (ctxl > 0 && r < 0.5) return "l" int(rand() * ctxl)
        if (r < 0.8) return "v" int(rand() * 4)
        return int(rand() * 10)
    }
    # call to a previous function; arguments are one level shallower, so
    # the recursion always ends
    function call(d,   g, s, a) {
        if (cur == 0 || d <= 0) return leaf()
        g = int(rand() * cur)
        s = "f" g "("
        for (a = 0; a < ar[g]; a++) s = s (a ? ", " : "") expr(d - 1)
        return s ")"
    }
    function expr(d,   r, op) {
        if (d == 0 || rand() < 0.25) return (rand() < 0.2 && cur > 0) ? call(1) : leaf()
        r = int(rand() * 8)
        if (r == 5) return "(" expr(d - 1) " / " (int(rand() * 9) + 1) ")"
        if (r >= 6) return (cur > 0 && rand() < 0.6) ? call(d) : "(" expr(d - 1) " + " expr(d - 1) ")"
        op = substr("+-*<>=", r + 1, 1)
        if (op == "=") op = "=="
        return "(" expr(d - 1) " " op " " expr(d - 1) ")"
    }
    # statement inside a function: assignment to a local or global, or if
    function fstmt(   t, n) {
        t = int(rand() * 3)
        if (t == 0) { print "v" int(rand() * 4) " = " expr(2) ";"; return }
        if (t == 1 && ctxl > 0) { print "l" int(rand() * ctxl) " = " expr(2) ";"; return }
        print "if " expr(2) " {"
        print "v" int(rand() * 4) " = " expr(1) ";"
        print "} else {"
        print "v" int(rand() * 4) " = " expr(1) ";"
        print "}"
    }
    function stmt(d,   r, k, n) {
        r = int(rand() * 3)
        if (d >= 3) r = 0
        if (r == 0) { print "v" int(rand() * 4) " = " expr(3) ";"; return }
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
    }' > "$OUT/fun_rand$i.fun"
    expected="$(timeout 20 "$BIN" --eval "$OUT/fun_rand$i.fun" 2>/dev/null | tail -1)"
    if [ -z "$expected" ]; then
        echo "FAIL $OUT/fun_rand$i.fun: interpretador nao produziu valor"
        fail=1
    else
        check_valid "$OUT/fun_rand$i.fun" "$expected"
    fi
    i=$((i + 1))
done

if [ $fail -eq 0 ]; then echo "Todos os testes passaram."; else echo "HOUVE FALHAS."; fi
exit $fail
