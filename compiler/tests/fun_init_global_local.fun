fun q(x) { return x * x; }
var a = q(6);
var b = q(a) - a;
fun h(x) {
  var l = q(x) + a;
  var m = l + q(2);
  return m;
}
main {
  return h(3) * 10000 + b;
}
