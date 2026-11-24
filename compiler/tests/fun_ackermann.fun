fun ack(m, n) {
  var r = 0;
  if m == 0 {
    r = n + 1;
  } else {
    if n == 0 {
      r = ack(m - 1, 1);
    } else {
      r = ack(m - 1, ack(m, n - 1));
    }
  }
  return r;
}
main {
  return ack(2, 3) * 100 + ack(3, 3);
}
