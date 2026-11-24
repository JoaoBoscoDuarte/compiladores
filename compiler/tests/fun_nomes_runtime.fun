var buffer = 1;
var Lfim0 = 2;
fun sair(Linicio0) {
  var Lfalso0 = Linicio0 + buffer;
  return Lfalso0 + Lfim0;
}
fun Lfim1(x) { return x * 2; }
main {
  return sair(10) * 100 + Lfim1(buffer);
}
