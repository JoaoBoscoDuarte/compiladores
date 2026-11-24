var cont = 0;
var t = 0;
fun incr(n) {
  cont = cont + n;
  return cont;
}
main {
  t = incr(3);
  t = incr(4);
  return cont * 100 + incr(0);
}
