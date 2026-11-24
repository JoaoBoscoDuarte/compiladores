var x = 100;
var y = 7;
fun f(x) {
  var y = x * 2;
  x = x + 1;
  return x * 1000 + y;
}
fun g() {
  var x = 1;
  return x + y;
}
main {
  return f(5) * 10000 + g() * 100 + x;
}
