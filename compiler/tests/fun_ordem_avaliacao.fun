var g = 0;
var r1 = 0;
var r2 = 0;
var r3 = 0;
fun mark(v) {
  g = g * 10 + v;
  return g;
}
fun pair(a, b, c) { return a * 100 + b * 10 + c; }
main {
  r1 = pair(mark(1), mark(2), mark(3));
  g = 0;
  r2 = mark(4) < mark(5);
  g = 0;
  r3 = mark(1) * 10 + mark(2);
  return r1 * 1000 + r2 * 100 + r3;
}
