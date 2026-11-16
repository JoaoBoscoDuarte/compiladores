n = 2;
cont = 0;
d = 0;
primo = 0;
{
  while n < 100 {
    primo = 1;
    d = 2;
    while d * d < n + 1 {
      if (n / d) * d == n {
        primo = 0;
      } else {
      }
      d = d + 1;
    }
    if primo {
      cont = cont + 1;
    } else {
    }
    n = n + 1;
  }
  return cont;
}
