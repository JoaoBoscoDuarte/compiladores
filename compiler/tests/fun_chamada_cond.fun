var i = 0;
var pares = 0;
fun par(n) {
  return n - (n / 2) * 2 == 0;
}
main {
  while i < 20 {
    if par(i) {
      pares = pares + i;
    } else {
    }
    i = i + 1;
  }
  return pares;
}
