i = 0;
pares = 0;
impares = 0;
{
  while i < 20 {
    if (i / 2) * 2 == i {
      pares = pares + i;
    } else {
      impares = impares + i;
    }
    i = i + 1;
  }
  return pares * 1000 + impares;
}
