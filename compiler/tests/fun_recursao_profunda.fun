fun soma(n) {
  var r = 0;
  if n == 0 {
    r = 0;
  } else {
    r = n + soma(n - 1);
  }
  return r;
}
main {
  return soma(3000);
}
