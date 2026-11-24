fun h(x) {
  var t = x + 1;
  return t * 2;
}
fun g(x) {
  var u = h(x) + 3;
  return u - 1;
}
fun f(x) {
  return g(x) * g(x + 1);
}
main {
  return f(4);
}
