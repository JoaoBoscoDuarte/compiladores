Programa valido (constante simples):

  $ "$TESTDIR"/../bin/ec1 "$TESTDIR"/valid1.ec1
  -- tokens --
  <Numero, "333", 0>
  -- arvore --
  333
  -- resultado --
  333

Programa valido (expressao aninhada, exemplo do enunciado):

  $ "$TESTDIR"/../bin/ec1 "$TESTDIR"/valid2.ec1
  -- tokens --
  <ParenEsq, "(", 0>
  <Numero, "33", 1>
  <Soma, "+", 4>
  <ParenEsq, "(", 6>
  <Numero, "912", 7>
  <Mult, "*", 11>
  <Numero, "11", 13>
  <ParenDir, ")", 15>
  <ParenDir, ")", 16>
  -- arvore --
  (33 + (912 * 11))
  -- resultado --
  10065

Erro lexico (caractere invalido):

  $ "$TESTDIR"/../bin/ec1 "$TESTDIR"/invalid_lexico.ec1
  Erro lexico na posicao 5: caractere inesperado '#'
  [1]

Erro sintatico (numeros sem operador entre eles):

  $ "$TESTDIR"/../bin/ec1 "$TESTDIR"/invalid_sintatico.ec1
  -- tokens --
  <ParenEsq, "(", 0>
  <Numero, "3", 1>
  <Numero, "4", 3>
  <ParenDir, ")", 4>
  Erro sintatico na posicao 3: esperava um operador, encontrado '4'
  [1]
