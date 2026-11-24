fun dobro(x) { return x * 2; }
fun soma(a, b) { return a + b; }
fun sub(a, b) { return a - b; }
main {
  return soma(dobro(3), dobro(soma(1, 2))) * 100 + sub(dobro(dobro(dobro(2))), soma(1, 1));
}
