fun pot(b, e) {
  var r = 1;
  var i = 0;
  while i < e {
    r = r * b;
    i = i + 1;
  }
  return r;
}
fun sp(n) {
  var t = 0;
  var k = 1;
  while k < n + 1 {
    t = t + pot(k, 3);
    k = k + 1;
  }
  return t;
}
main {
  return sp(10);
}
