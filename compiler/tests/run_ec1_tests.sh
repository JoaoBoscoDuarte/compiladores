#!/bin/sh
# Smoke test for bin/ec1: valid programs must print the right value and
# exit 0; programs with lexical/syntax errors must exit non-zero.
set -eu

cd "$(dirname "$0")"
BIN="../bin/ec1"

fail=0

check_valid() {
    file="$1"; expected="$2"
    got="$("$BIN" "$file" | tail -1)"
    if [ "$got" != "$expected" ]; then
        echo "FAIL $file: esperava '$expected', obteve '$got'"
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

check_valid ec1_valid1.ec1 333
check_valid ec1_valid2.ec1 10065
check_invalid ec1_invalid_lexico.ec1
check_invalid ec1_invalid_sintatico.ec1

exit $fail
