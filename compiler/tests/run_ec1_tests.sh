#!/bin/sh
# Automatic tests for bin/ec1.
#  - valid programs: compile to assembly, assemble, link, run, and compare the
#    printed value with the expected one (and with the interpreter's value);
#  - invalid programs (lexical/syntax errors): the compiler must fail;
#  - large programs are generated on the fly (any size must work).
# Run from anywhere; requires `make` first plus `as` and `ld` (x86-64 Linux).
set -u

cd "$(dirname "$0")/.."
BIN="./bin/ec1"
OUT="asm"
mkdir -p "$OUT"

fail=0

# run_asm <file.ec1>: compiles, assembles, links and runs; prints the output.
run_asm() {
    name="$(basename "$1" .ec1)"
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

check_valid tests/ec1_valid1.ec1 333
check_valid tests/ec1_valid2.ec1 10065
check_valid tests/ec1_sub.ec1 4
check_valid tests/ec1_enunciado.ec1 42
check_valid tests/ec1_misto.ec1 2657
check_valid tests/ec1_ordem.ec1 9
check_valid tests/ec1_negativo.ec1 -3
check_valid tests/ec1_divisao.ec1 7
check_valid tests/ec1_zero.ec1 0
check_valid tests/ec1_espacos.ec1 7

check_invalid tests/ec1_invalid_lexico.ec1
check_invalid tests/ec1_invalid_sintatico.ec1
check_invalid tests/ec1_invalid_sintatico2.ec1
check_invalid tests/ec1_invalid_grande.ec1

# Large programs (generated): 3000 nested operations, two shapes.
N=3000
# left-nested: ((((1 + 1) + 1) + 1) ... ) -> N + 1
awk -v n=$N 'BEGIN { for (i=0;i<n;i++) printf "("; printf "1"; for (i=0;i<n;i++) printf " + 1)"; printf "\n" }' > "$OUT/ec1_grande_esq.ec1"
# right-nested: (1 + (1 + (1 + ... 1))) -> N + 1
awk -v n=$N 'BEGIN { for (i=0;i<n;i++) printf "(1 + "; printf "1"; for (i=0;i<n;i++) printf ")"; printf "\n" }' > "$OUT/ec1_grande_dir.ec1"
check_valid "$OUT/ec1_grande_esq.ec1" $((N + 1))
check_valid "$OUT/ec1_grande_dir.ec1" $((N + 1))

# Random programs: assembly result must match the interpreter.
i=0
while [ $i -lt 30 ]; do
    awk -v seed=$i 'BEGIN {
        srand(seed + 1)
        print gen(6)
    }
    function gen(d,   r, op, o) {
        if (d == 0 || rand() < 0.25) return int(rand() * 50) + 1
        r = int(rand() * 4)
        op = substr("+-*/", r + 1, 1)
        return "(" gen(d - 1) " " op " " gen(d - 1) ")"
    }' > "$OUT/ec1_rand$i.ec1"
    expected="$("$BIN" --eval "$OUT/ec1_rand$i.ec1" 2>/dev/null | tail -1)"
    if [ -z "$expected" ] || [ "$("$BIN" --eval "$OUT/ec1_rand$i.ec1" 2>&1 >/dev/null)" ]; then
        # division by zero in the generated program: skip this sample
        i=$((i + 1)); continue
    fi
    check_valid "$OUT/ec1_rand$i.ec1" "$expected"
    i=$((i + 1))
done

if [ $fail -eq 0 ]; then echo "Todos os testes passaram."; else echo "HOUVE FALHAS."; fi
exit $fail
