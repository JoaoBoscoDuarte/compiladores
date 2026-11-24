fun abs(x) {
  var y = 0;
  if x < 0 {
    y = 0 - x;
  } else {
    y = x;
  }
  return y;
}
main {
  return abs(0 - 17) * 100 + abs(5);
}
