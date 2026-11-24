fun par(n) {
  var r = 0;
  if n == 0 { r = 1; } else { r = impar(n - 1); }
  return r;
}
fun impar(n) {
  var r = 0;
  if n == 0 { r = 0; } else { r = par(n - 1); }
  return r;
}
main { return par(4); }
