x = 0 - 5;
y = 3;
z = 0;
{
  if x < y {
    z = x * y;
  } else {
    z = 1;
  }
  while x < 0 {
    x = x + 2;
  }
  return z * 100 + x;
}
