#!/bin/sh
# Automatic tests for bin/ev.
#  - valid programs: compile to assembly, assemble, link, run, and compare the
#    printed value with the expected one (and with the interpreter's value);
#  - invalid programs (lexical/syntactic/semantic errors): the compiler must fail;
#  - large programs are generated on the fly (any size must work).
# Run from anywhere; requires `make` first plus `as` and `ld` (x86-64 Linux).
set -u

cd "$(dirname "$0")/.."
BIN="./bin/ev"
OUT="asm"
mkdir -p "$OUT"

fail=0

# run_asm <file.ev>: compiles, assembles, links and runs; prints the output.
run_asm() {
    name="$(basename "$1" .ev)"
    "$BIN" "$1" >/dev/null || return 1
    as --64 -o "$OUT/$name.o" "$OUT/$name.s" || return 1
    ld -o "$OUT/$name" "$OUT/$name.o" || return 1
    "$OUT/$name"
}

check_valid() {
    file="$1"; expected="$2"
    got="$(run_asm "$file")"
    interp="$("$BIN" --eval "$file" | tail -1)"
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

check_invalid() {
    file="$1"
    if "$BIN" "$file" >/dev/null 2>&1; then
        echo "FAIL $file: deveria ter falhado"
        fail=1
    else
        echo "ok   $file -> rejeitado corretamente"
    fi
}

# Programas validos: declaracoes de variaveis + expressao final.
check_valid tests/ev_perimetro.ev 140
check_valid tests/ev_exemplo_pdf.ev 60467
check_valid tests/ev_sem_decl.ev 21

# Programas invalidos: erro lexico, sintatico e semantico (variavel nao
# declarada, inclusive quando declarada so depois de usada).
check_invalid tests/ev_invalid_lexico_ident.ev
check_invalid tests/ev_invalid_nao_declarada.ev
check_invalid tests/ev_invalid_ordem.ev

# Large programs (generated): 3000 nested operations, two shapes, no variables.
N=3000
# left-nested: = ((((1 + 1) + 1) + 1) ... ) -> N + 1
awk -v n=$N 'BEGIN { printf "= "; for (i=0;i<n;i++) printf "("; printf "1"; for (i=0;i<n;i++) printf " + 1)"; printf "\n" }' > "$OUT/ev_grande_esq.ev"
# right-nested: = (1 + (1 + (1 + ... 1))) -> N + 1
awk -v n=$N 'BEGIN { printf "= "; for (i=0;i<n;i++) printf "(1 + "; printf "1"; for (i=0;i<n;i++) printf ")"; printf "\n" }' > "$OUT/ev_grande_dir.ev"
check_valid "$OUT/ev_grande_esq.ev" $((N + 1))
check_valid "$OUT/ev_grande_dir.ev" $((N + 1))

# Random programs (constants only, no variables): assembly result must
# match the interpreter.
i=0
while [ $i -lt 30 ]; do
    awk -v seed=$i 'BEGIN {
        srand(seed + 1)
        printf "= "
        print gen(6)
    }
    function gen(d,   r, op, o) {
        if (d == 0 || rand() < 0.25) return int(rand() * 50) + 1
        r = int(rand() * 4)
        op = substr("+-*/", r + 1, 1)
        return "(" gen(d - 1) " " op " " gen(d - 1) ")"
    }' > "$OUT/ev_rand$i.ev"
    eval_out="$("$BIN" --eval "$OUT/ev_rand$i.ev" 2>/dev/null)"
    eval_status=$?
    expected="$(printf '%s\n' "$eval_out" | tail -1)"
    if [ $eval_status -ne 0 ]; then
        # division by zero in the generated program (interpreter crashes with
        # SIGFPE, no stderr output): skip this sample
        i=$((i + 1)); continue
    fi
    check_valid "$OUT/ev_rand$i.ev" "$expected"
    i=$((i + 1))
done

if [ $fail -eq 0 ]; then echo "Todos os testes passaram."; else echo "HOUVE FALHAS."; fi
exit $fail
