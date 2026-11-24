fun mdc(a, b) {
  var r = 0;
  if b == 0 {
    r = a;
  } else {
    r = mdc(b, a - (a / b) * b);
  }
  return r;
}
main {
  return mdc(1071, 462) * 1000 + mdc(18, 12);
}
