x = 5;
y = 0;
{
  if x > 3 {
    if x < 5 {
      y = 1;
    } else {
      if x == 5 {
        y = 2;
      } else {
        y = 3;
      }
    }
  } else {
    y = 4;
  }
  return y;
}
