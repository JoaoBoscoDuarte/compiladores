buffer = 1;
sair = 2;
Lfim0 = 3;
Linicio0 = 4;
Lfalso0 = 5;
xvar = 7;
{
  if buffer < sair {
    Lfim0 = Lfim0 + Linicio0 + Lfalso0 + xvar;
  } else {
    Lfim0 = 0;
  }
  while buffer < 3 {
    buffer = buffer + 1;
  }
  return Lfim0 * 10 + buffer;
}
