var n = 0;
var s = 0;
fun quad(x) { return x * x; }
main {
  while quad(n) < 200 {
    s = s + quad(n);
    n = n + 1;
  }
  return s * 1000 + n;
}
