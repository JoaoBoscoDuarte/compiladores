i = 0;
a = 0;
b = 1;
t = 0;
{
  while i < 30 {
    t = a + b;
    a = b;
    b = t;
    i = i + 1;
  }
  return a;
}
